;MBR. Loads file LOADER from root directory to address 1000h and run him.
;(C) Alekzzz

USE16
ORG 0x7C00

LOADER_MEMORY equ 1000h   ;Second loader address (4kb)
ldr_clust equ bpbOEM
FirstDataSec equ bpbVolLabel32

boot:
JMP start		  ;JMP to start
NOP

bpbOEM		db  'ASDF0.01' ;         +03h
bpbSectSize	dw  200h       ;512      +0Bh
bpbClustSize	db  8h	       ;8        +0Dh
bpbReservedSec	dw  22h        ;38       +0Eh
bpbFats 	db  2h	       ;2        +10h
bpbRootSize	dw  0h	       ;0        +11h
bpbTotalSect	dw  0h	       ;0        +13h
bpbMedia	db  0F8h       ;248      +15h
bpbFatSize	dw  0h	       ;0        +16h
bpbTrackSect	dw  3Fh        ;63       +18h
bpbHeads	dw  0FFh       ;255      +1Ah
bpbHiddenSect	dd  20h        ;0        +1Ch
bpbLargeSect	dd  0F5FE0h    ;7987200  +20h

bpbSectPerFAT32        dd  3D7h
bpbFATFlags32	       dw  0h
bpbVersion32	       dw  0h
bpbRootDir1stClust32   dd  2h
bpbFSInfSect32	       dw  1h
bpbBackupBootSector32  dw  6h
bpbReserved32	       rb  12
bpbDriveNo32	       db  0h
pbpUnused32	       db  0
bpbExBootSign32        db  29h
bpbVolSN32	       dd  04D97D2Ch
bpbVolLabel32	       db  'NO NAME    '
pbpFSType32	       db  'FAT32   '

start:
 CLI		;Disable interrupt
 XOR AX,AX
 MOV SS,AX
 MOV ES,AX
 MOV DS,AX
 MOV SP,boot   ;Stack initialization
 STI		;Enable interrupt

 MOV [bpbDriveNo32],DL	;Boot disk No

;SHR DL,4
;ADD DL,30h
;MOV byte [pbpFSType32],DL
;MOV DL,[bpbDriveNo32]
;AND DL,0Fh
;ADD DL,30h
;MOV byte [pbpFSType32+1],DL
;MOV byte [pbpFSType32+2],0

 ;Set text-mode 80x25 16 colors
 MOV AX,3
 INT 10h
;MOV SI,pbpFSType32
;call puts

 ;Calculating Root sector in EAX (EAX=FATCount*FATsz+ReservedSec)
 XOR EAX,EAX
 MOV AL,byte [bpbFats]			;AL=FAT's count
 MUL dword [bpbSectPerFAT32]		;AX*=FAT size 32
 XOR EBX,EBX
 MOV BX,[bpbReservedSec]
 ADD EAX,EBX				;AX+=Reserved sectors; AX=Root Sector LBA
 MOV dword [FirstDataSec],EAX
;***********************************************************

 MOV SI,buff
 MOV BL,[bpbClustSize]
 CALL ReadLBASector
 MOV AX,0ABh
 JC error

 MOV AX,buff
 XOR BX,BX
 MOV BL,[bpbClustSize]
 SHL BX,4h		  ;Root directory entries (BPB_ClusterSize*512/32)

 search:
PUSHAD
MOV SI,secldr
call puts
POPAD
  TEST BX,BX
  JC error		  ;If end of root directory then error
  MOV SI,AX
  CMP [SI],byte 0
  JZ error
  MOV DI,secldr
  MOV CX,11
  REP CMPSB
  JZ founded
   ADD AX,20h
   DEC BX
   JMP search

founded:
 MOV SI,AX
 MOV AX,word [SI+14h]	       ;High word
 SHL EAX,10h
 MOV AX,word[SI+1Ah]	       ;Low word. EAX=First cluster of LOADER
 MOV dword [ldr_clust],EAX

 SHR EAX,7h
 XOR EBX,EBX
 MOV BX,[bpbReservedSec]
 ADD EAX,EBX		       ;Number of FAT sector included LOADER (N*4/BPB_BytesPerSec)+BPB_ReservedSecCnt

 ;Reading FAT
 MOV SI,buff
 MOV BL,[bpbClustSize]
 CALL ReadLBASector
 MOV AX,0ABh
 JC error

 ;LOADER reading loop
 MOV ESI,LOADER_MEMORY
 loader_reading_loop:
 ;FirstSectorofCluster = ((N - 2) * BPB_SecPerClus) + FirstDataSector
  MOV EAX,dword [ldr_clust]
  SUB EAX,2h			;FirstSectorofCluster=N-2
  XOR EBX,EBX
  MOV BL,[bpbClustSize]
  MUL EBX
  XOR EBX,EBX
  MOV BX,word [FirstDataSec]
  ADD EAX,EBX			;FirstSectorofCluster+=FirstDataSector

 ;**************************
  PUSH ESI			 ;Store Loader buffer address
  MOV BL,[bpbClustSize]
  CALL ReadLBASector
  MOV AX,0ABh
  JC error

  POP ESI
  ADD ESI,1000h

  MOV EAX,dword [ldr_clust]
  SHL EAX,2
  MOV EBX,EAX
  SHR EBX,9
  SHL EBX,9
  SUB EAX,EBX			;Loader cluster offset in FAT

  MOV EBX,[buff+EAX]
  CMP EBX,0FFFFFFFh		;if last cluster
  JMP LOADER_MEMORY		;then run loader
  MOV dword[ldr_clust],EBX
  JMP loader_reading_loop	;else repeat
;****************************************************

error:
 MOV SI,loader_notfound
 CMP AX,0ABh
 JNZ error_print_loop
 MOV SI,io_err
 error_print_loop:
  LODSB
  TEST AL,AL
   JZ error_ret
  XOR BH,BH
  MOV AH,0Eh
  INT 10h
  JMP error_print_loop
 error_ret:
JMP $

puts:;SI - Null terminated string
 LODSB
 TEST AL,AL
  JZ puts_ret
 XOR BH,BH
 MOV AH,0Eh
 INT 10h
 JMP puts
puts_ret: ret

;****************************************************
ReadLBASector:	;EAX - LBA, BX - Sectors count, SI- Buffer address,byte bpbDriveNo32 - Disk #
 PUSH BX

XOR EDX,EDX
XOR EBX,EBX
MOV BX,[bpbTrackSect]
DIV EBX
INC EDX
PUSH DX ;DL = Sect

XOR EDX,EDX
XOR EBX,EBX
MOV BX,[bpbHeads]
DIV EBX

POP BX
MOV CL,BL		   ;Sector
MOV CH,AL		   ;Track
MOV DH,DL		   ;Head
MOV DL,byte [bpbDriveNo32] ;Drive

POP AX			   ;Sectors count
MOV AH,02h
MOV BX,SI
MOV DI,5h

INT 13h
RET

; XOR ECX,ECX
; PUSH ECX ;LBA
; PUSH EAX

; PUSH CX  ;Buffer
; PUSH SI

; XOR BH,BH
; PUSH BX  ;Sectors count
; PUSH word 10h

; MOV DI,5h

; ReadLBASector_repeat:
;  MOV DL,[bpbDriveNo32]
;  MOV SI,SP
;  MOV AH,42h
;  INT 13h
;  JNC ReadLBASector_exit_success
;  DEC DI
; JNZ ReadLBASector_repeat
; ReadLBASector_exit_success:

; ADD SP,10h
; RET
;*************************************************************************************

loader_notfound db 'LOADER not found',0
io_err db 'I/O error',0
secldr db 'LOADER     '

times 510-($-$$) db 0

bootsign dw 0AA55h
buff: