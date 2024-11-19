// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

// #include "ioports.h"

__inline__ void outportb(unsigned short port, unsigned char value) {
    asm volatile("outb %b0,%w1"::"a"(value), "d"(port));
}

__inline__ char inportb(unsigned short port) {
    char value;
    asm volatile("inb %w1, %b0":"=a"(value):"d"(port));
    return value;
}

__inline__ void outportw(unsigned short port, unsigned short value) {
    asm volatile("outw %w0,%w1"::"a"(value), "d"(port));
}

__inline__ short inportw(unsigned short port) {
    short value;
    asm volatile("inw %w1,%w0":"=a"(value):"d"(port));
    return value;
}
