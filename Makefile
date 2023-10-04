kernel := kernel.elf
iso := build/asdfos.iso
linker_script := linker.ld
grub_cfg := grub.cfg

source_files := $(wildcard kernel/*.c)
object_files := $(patsubst %.c, %.o, $(source_files))

CC=gcc
CFLAGS=-m32 -fno-pie -ffreestanding -c -fno-omit-frame-pointer -fno-stack-protector -fno-asynchronous-unwind-tables -Wall

default: $(kernel)

clean:
	- @rm -fr build
	- @rm -f kernel/*.o
	- @rm $(kernel)

qemu: $(iso)
	qemu-system-i386 -cdrom $(iso) -vga std -s -monitor stdio -accel kvm

qemu-gdb: $(iso)
	qemu-system-i386 -cdrom $(iso) -vga std -s -S

bochs: $(iso)
	bochs -f bochs.bxrc

iso: $(iso)

$(iso): $(kernel) $(grub_cfg)
	mkdir -p build/isofiles/boot/grub
	cp $(kernel) build/isofiles/boot/$(kernel)
	cp $(grub_cfg) build/isofiles/boot/grub
	grub-mkrescue -o $(iso) build/isofiles #2> /dev/null

$(kernel): $(object_files)
	ld -m elf_i386 -n -T $(linker_script) -o $(kernel) $(object_files)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

d: $(kernel)
	objdump -d $(kernel) | less

b: bochs

c: clean

i: iso

q: qemu
