USE16
ORG 1000h
;ORG 100h
;jmp start
;bpbOEM          db  'ASDF0.01' ;       +03h
;bpbSectSize     dw  200h       ;512    +0Bh
;bpbClustSize    db  1h         ;1      +0Dh
;bpbReservedSec  dw  1h         ;1      +0Eh
;bpbFats         db  2h         ;2      +10h
;bpbRootSize     dw  00E0h      ;224    +11h
;bpbTotalSect    dw  0B40h      ;2880   +13h
;bpbMedia        db  0F0h       ;240    +15h
;bpbFatSize      dw  9h         ;9      +16h
;bpbTrackSect    dw  12h        ;18     +18h
;bpbHeads        dw  2h         ;2      +1Ah
;bpbHiddenSect   dd  0h         ;0      +1Ch
;bpbLargeSect    dd  0h         ;0      +20h

;bpbDriveNo      db  0h         ;0      +24h
;bpbReserved     db  0h         ;0      +25h
;bpbSignature    db  29h        ;41     +26h
;bpbID           dd  1h         ;1      +27h
;bpbVolumeLabel  db  'ASDFOS 0.01';     +2Bh
;bpbFileSystem   db  'FAT12   ';        +36h

bpbOEM		equ   603h ;8
bpbSectSize	equ   60Bh ;dw
bpbClustSize	equ   60Dh ;db
bpbReservedSec	equ   60Eh ;dw
bpbFats 	equ   610h ;db
bpbRootSize	equ   611h ;dw
bpbTotalSect	equ   613h ;dw
bpbMedia	equ   615h ;db
bpbFatSize	equ   616h ;dw
bpbTrackSect	equ   618h ;dw
bpbHeads	equ   61Ah ;dw
bpbHiddenSect	equ   61Ch ;dd
bpbLargeSect	equ   620h ;dd

bpbDriveNo	equ   624h ;db
bpbReserved	equ   625h ;db
bpbSignature	equ   626h ;db
bpbID		equ   627h ;dd
bpbVolumeLabel	equ   62Bh ;db[11]
bpbFileSystem	equ   636h ; db[8]

DataSector equ bpbOEM
ldrsec equ bpbOEM+2

KERNEL equ 2000h
KERNEL_ADDR equ 200000h

start:
 CLI		;Отключить прерывания
 MOV AX,0
; MOV AX,CS      ;Копируем в AX содержимое сегментного регистра CS
 MOV ES,AX
 MOV DS,AX	;Инициализация остальных
 MOV SS,AX	;сегментных регистров
 MOV SP,start	 ;Инициализация стека
 STI		;Включить прерывания
;MOV [bpbDriveNo],byte 0
 ;MOV AX,3
 ;INT 10h

 MOV SI,7C00h
 MOV DI,600h
 MOV CX,40h
 REP MOVSW

 PUSH col
 PUSH row
 PUSH 0Ah
 PUSH msg_readfat
 CALL puts

 ;Calculating Root sector in AX
 MOV AL,byte [bpbFats]			;AL=FAT's count
 CBW
 MUL word [bpbFatSize]			;AX*=FAT size 12
 ADD AX,word [bpbHiddenSect]		;AX+=hiden sectors
 ADC AX,word [bpbHiddenSect+2]
 ADD AX,[bpbReservedSec]		;AX+=Reserved sectors; AX=Root Sector LBA

 PUSH buff		  ;Buffer
 PUSH word [bpbDriveNo]   ;Disk
 PUSH word 1		  ;Sectors count
 PUSH AX
 CALL ReadLBASector
 JC io_error

 MOV AX,buff
 MOV BX,[bpbRootSize]

 PUSH col
 PUSH row
 PUSH 0Ah
 PUSH msg_knl_sch
 CALL puts

; SHR BX,4            ;Root size = (RootEnt*32)/512
 search:
  TEST BX,BX
  JC knl_notfound
  MOV SI,AX
  CMP [SI],byte 0
  JNZ not_end
   JMP knl_notfound
  not_end:
  MOV DI,kernel_name
  MOV CX,11
  REP CMPSB
  JZ founded
   ADD AX,20h
   DEC BX
   JMP search

founded:
 PUSH col
 PUSH row
 PUSH 0Ah
 PUSH msg_knl_loading1
 CALL puts

 MOV SI,AX
 MOV AX,[SI+1Ch] ;kernel size

 PUSH msg_readfat
 PUSH AX
 CALL itoa

 PUSH col
 PUSH row
 PUSH 0Ah
 PUSH msg_readfat
 CALL puts

 PUSH col
 PUSH row
 PUSH 0Ah
 PUSH msg_knl_loading2
 CALL puts

;Calculating Data sector
;FATSect = FATCount * FATsize
;RootData = (RootSize*32 + (SectPerClust-1)) / SectPerClust
;Datasector = Hidden + Reserved + FATSect + RootData

 MOV AX,[bpbRootSize]
 SHL AX,5h
 ADD AX,[bpbSectSize]
 DEC AX
 DIV word [bpbSectSize]
 XOR AH,AH
 PUSH AX ;RootData

 MOV AL,[bpbFats]
 MUL word [bpbFatSize] ;FATSect

 ADD AX,word [bpbHiddenSect]
 ADD AX,[bpbReservedSec]
 POP BX
 ADD AX,BX
 SUB AX,2h
 MOV word[DataSector],AX

 MOV AX,[SI+1Ah]
 MOV word[ldrsec],AX

;Reading FAT Table
 PUSH buff		  ;Buffer
 PUSH word [bpbDriveNo]   ;Disk
 PUSH word 9		  ;Sectors count
 PUSH 1
 CALL ReadLBASector
 JC io_error

 MOV DI,KERNEL

;Reading kernel
 read_secondldr:
  MOV SI,buff
  MOV AX,word[ldrsec]
  ADD AX,word[DataSector]
  PUSH DI		   ;Buffer
  PUSH word [bpbDriveNo]   ;Disk
  PUSH word 1h		   ;Sectors count
  PUSH AX
  CALL ReadLBASector
  JC io_error

  MOV AX,word[ldrsec]
  MOV BX,3
  MUL BX
  SHR AX,1

  ADD SI,AX
  MOV AX,[SI]
  TEST word [ldrsec],1h
  JZ chet
   SHR AX,4
   JMP cont
  chet:
   AND AX,0FFFh
  cont:
  CMP AX,0FFFh
  JZ go_protected	;if all kernel loaded go protected mode
  CMP AX,0FF0h
  JZ go_protected
  CMP AX,0FF0Fh
  JZ go_protected
  MOV word[ldrsec],AX
  ADD DI,200h

PUSH col
PUSH row
PUSH 0Ah
PUSH dot
CALL puts

 JMP read_secondldr

 go_protected:	    ;Enter protected mode

 PUSH col
 PUSH row
 PUSH 0Ah
 PUSH msg_knl_start
 CALL puts

 MOV DX,3f2h ;
 XOR AL,AL   ;Stop FDD motor
 OUT DX,AL   ;

 ;Setting base vector of interrupt controller in 20h
 MOV AL,00010001b
 OUT 20h,AL
 MOV AL,20h
 OUT 21h,AL
 mov al,00000100b
 OUT 21h,AL
 MOV AL,00000001b
 OUT 21h,AL

 MOV AL,00010001b
 OUT 0A0h,AL
 MOV AL,28h
 OUT 0A1h,AL
 mov al,00000100b
 OUT 0A1h,AL
 MOV AL,00000001b
 OUT 0A1h,AL

 CLI	      ;Disable HW interrupt

 IN AL,70h    ;
 OR AL,80h    ;Disable NMI
 OUT 70h,AL   ;

 LGDT [gd_reg] ;Load GDTR

 IN AL,92h
 OR AL,2       ;Open address line A20
 OUT 92h,AL

 MOV EAX,CR0	 ;
 OR AL,1	 ;Setting PE bit
 MOV CR0,EAX	 ;
 JMP 8h:_protected


ReadLBASector: ;readSector(WORD LBASector,WORD SectorsCount,BYTE disk,char *buffer[BytesPerSector*SectorsCount])
 PUSH BP
 MOV BP,SP
 PUSHA

 MOV AX,[bpbTrackSect]
 MUL word [bpbHeads]
 XCHG AX,BX
 MOV AX,[BP+4h]
 DIV BX
 MOV CH,AL
 ;tmp=DX=LBA%(heads*sector_per_track)
 MOV SI,DX
 ;head=tmp/sector_per_track
 MOV AX,SI
 XOR DX,DX
 DIV word [bpbTrackSect]
 MOV DH,AL
 ;sector=tmp%sector_per_track+1
 MOV CL,DL
 INC CL

 MOV DL,[BP+8h]
 MOV BX,[BP+0Ah]
 MOV DI,5h

 ReadLBASector_repeat:
  MOV AL,[BP+6h]
  MOV AH,2h
  INT 13h
  JNC ReadLBASector_exit_success
  DEC DI
 JNZ ReadLBASector_repeat

 POPA
 POP BP
 RET 8
 ReadLBASector_exit_success:
  POPA
  POP BP
  RET 8
;****************************************************

puts: ; puts(char *str,BYTE arttribute,WORD row,WORD col)
 PUSH BP
 MOV BP,SP
 PUSHA

 PUSH ES
 PUSH 0B800h
 POP ES

 MOV SI,[BP+4]	 ;String
 MOV AX,[BP+6]	 ;Attribute
 SHL AX,8	 ;Attribute high byte

 new_line:
 MOV DI,[BP+8]	 ;Row
 MOV DI,[DI]
 IMUL DI,0A0h
 MOV BX,DI

 MOV DI,[BP+0Ah] ;Col
 MOV DI,[DI]
 SHL DI,1
 ADD DI,BX	 ;Setting position
 SUB DI,2
 puts_loop:
  LODSB
  TEST AL,AL
  JE puts_exit
  ADD DI,2
  PUSH DI
  MOV DI,[BP+0Ah]
  INC word [DI]
  POP DI
  CMP AL,0Ah
  JNZ puts_cont
   MOV DI,[BP+8h]
   INC word [DI]
   MOV DI,[BP+0Ah]
   MOV [DI],word 0
   JMP new_line
  puts_cont:
  MOV [ES:DI],AX
  JMP puts_loop
 puts_exit:
 POP ES
 POPA
 POP BP
RET 8

itoa: ; itoa(unsigned short value,char *str)
 PUSH BP
 MOV BP,SP
 PUSHA

 MOV AX,[BP+4]	;Value
 MOV SI,[BP+6]	;Str

 MOV BX,0Ah

 itoa_loop:
  XOR DX,DX
  DIV BX
  ADD DL,'0'
  MOV [SI],DL
  INC SI
  TEST AX,AX
 JNZ itoa_loop
 MOV [SI],byte 0
 DEC SI

 MOV DI,[BP+6]

 itoa_revers:
  MOV BL,[DI]
  XCHG BL,[SI]
  XCHG [DI],BL
  DEC SI
  INC DI
  CMP DI,SI
 JB itoa_revers

 POPA
 POP BP
RET 4

io_error:
 PUSH col
 PUSH row
 PUSH 0Ch
 PUSH msg_ioerror
 CALL puts
 JMP $

knl_notfound:
 PUSH col
 PUSH row
 PUSH 0Ch
 PUSH msg_knl_notfound
 CALL puts
 JMP $

;error:JMP $

USE32
_protected:
 MOV AX, 10h
 MOV DS, AX
 MOV ES, AX
 MOV FS, AX
 MOV GS, AX
 MOV SS, AX

 MOV ESI,KERNEL
 MOV EDI,KERNEL_ADDR
 MOV ECX,100000
 REP MOVSD
 ;MOV ESP,KERNEL_ADDR-4
 JMP 8h:KERNEL_ADDR;!0x008

kernel_name db 'KERNEL  KNL'

msg_readfat db 'Reading FAT',0Ah,0
msg_knl_sch db 'Searching KERNEL.KNL',0Ah,0
msg_knl_loading1 db 'Loading ',0
msg_knl_loading2 db ' bytes to memory (512 bytes per read)',0Ah,0
msg_knl_start db 0Ah,'Runing kernel',0Ah,0
msg_knl_notfound db 'Kernel not found',0Ah,0
msg_ioerror db 'I/O error',0Ah,0
dot db '* ',0
str_sector db 'sector',0Ah,0

row dd 0
col dd 0
knl_size dd 0

;GDT C-style format
;struct GDT
;{
;unsigned low_limit:16;   //Low 16 bits of limit
;unsigned low_base:24;    //Low 24 bits of base
;unsigned Accessed:1;     //Segment accessed
;unsigned EW:1;           //if CD==0 Writeble 0/1 - Y/N | if CD==1 Readable 0/1 - Y/N
;unsigned EC:1;           //if CD==0 Expansion direction 1/0 - Stack/default | if CD==1 Conforming
;unsigned CD:1;           //0/1 - Data/code (if Sys==1)
;unsigned Sys:1;          //0/1 - Segment system/data or code
;unsigned DPL:2;          //Descriptor Privilege Level
;unsigned present:1;      //0/0 - Segment not present/present
;unsigned high_limit:4;   //High 4 bits of limit
;unsigned avl:1;          //Available for system software
;unsigned reserved:1;     //May be 0
;unsigned SB:1;           //if CD==1 0/1 - 16/32 bits code | if CD==0 0/1 - ?/Big
;unsigned Granularity:1;  //0/1 - 1 bit/4096 bits
;unsigned high_base:8;    //High 8 bits of base
;}
gdt:
dw 0, 0, 0, 0 ; Null descriptor
;Code segment
dw 0xFFFF	;Limit *FFFFh (bits 0-15)
dw 0x0000	;Base **000000h (bits 0-15)
db 0x00 	;Base (bits 16-23)
db 10011010b	;Present | DPL=00 | Sys=1 || Code-Non writable-As stack|Accessed
db 0xCF 	;Granularity | 32bits | Limit F****h (bits 16-19)
db 0x00 	;Base 00******h (bits 24-31)

;Data segment
dw 0xFFFF	;Limit *FFFFh (bits 0-15)
dw 0x0000	;Base **000000h (bits 0-15)
db 0x00 	;Base (bits 16-23)
db 10010010b	;Present | DPL=00 | Sys=1 || Data-Non writable-As stack|Accessed
db 0xCF 	;Granularity | 32bits | Limit F****h (bits 16-19)
db 0x00 	;Base 00******h (bits 24-31)

;GDTR:
gd_reg:
dw 8192
dd gdt

buff:

