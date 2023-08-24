OBJECTS=startup.o kernel.o syscounters.o interrupt.o tty.o vtty.o string.o keyboard.o floppy.o clock.o exceptions.o paging.o mem.o cpu.o gdt.o ke_syscall.o syscall.o shell.o stdio.o task.o
CFLAGS=-ffreestanding -c -m32
default:
	gcc $(CFLAGS) -o vtty.o vtty.c
	gcc $(CFLAGS) -o tty.o tty.c
	gcc $(CFLAGS) -o keyboard.o keyboard.c
	gcc $(CFLAGS) -o startup.o startup.c
	gcc $(CFLAGS) -o interrupt.o interrupt.c
	gcc $(CFLAGS) -o string.o string.c
	gcc $(CFLAGS) -o floppy.o floppy.c
	gcc $(CFLAGS) -o clock.o clock.c
	gcc $(CFLAGS) -o exceptions.o exceptions.c
	gcc $(CFLAGS) -o mem.o mem.c
	gcc $(CFLAGS) -o paging.o paging.c
	gcc $(CFLAGS) -o syscounters.o syscounters.c
	gcc $(CFLAGS) -o cpu.o cpu.c
	gcc $(CFLAGS) -o gdt.o gdt.c
	gcc $(CFLAGS) -o ke_syscall.o ke_syscall.c
	gcc $(CFLAGS) -o syscall.o syscall.c
	gcc $(CFLAGS) -o stdio.o stdio.c
	gcc $(CFLAGS) -o shell.o shell.c
	gcc $(CFLAGS) -o task.o task.c
	gcc $(CFLAGS) -o kernel.o kernel.c
	ld --oformat=binary -Ttext 0x100000 -o KERNEL.KNL $(OBJECTS)
	rm $(OBJECTS)
