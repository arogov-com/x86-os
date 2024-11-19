// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#define outportb(port, value) asm volatile("outb %%al, %%dx"::"a"(value), "d"(port))
#define outportw(port, value) asm volatile("outb %%ax, %%dx"::"a"(value), "d"(port))

#define inportb(port) ({unsigned char _inbval;asm volatile ("inb %%dx, %%al":"=a"(_inbval):"d"(port));_inbval;})
#define inportw(port) ({unsigned char _inbval;asm volatile ("inb %%dx, %%ax":"=a"(_inbval):"d"(port));_inbval;})
/*
__inline__ void outportb(unsigned short port,unsigned char value);
__inline__ char inportb(unsigned short port);
__inline__ void outportw(unsigned short port,unsigned short value);
__inline__ short inportw(unsigned short port);
*/
