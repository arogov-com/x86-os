// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include "ioports.h"
#include "floppy.h"
#include "clock.h"
#include "interrupt.h"
#include "stdio.h"

/*
  char portout;
  printf("Scaning devices...\n");
  portout=scan_flopies();
  printf("  fd0: %s\n  fd1: %s\n",drive_type[portout>>4],drive_type[portout&0xF]);

inportb(0x1F7);
outportb(0x1F6,0xE0);
printf("ad0:0 %x\n",inportb(0x1F7));

inportb(0x1F7);
outportb(0x1F6,0xF0);
printf("ad0:1 %x\n",inportb(0x1F7));

inportb(0x177);
outportb(0x176,0xE0);
printf("ad1:0 %x\n",inportb(0x177));
inportb(0x177);
outportb(0x176,0xF0);
printf("ad1:1 %x\n",inportb(0x177));
*/

char floppies=-1;
int irq_fdc_wait=0;

typedef struct {
    int heads_per_cyinder,
        sector_per_track;
}discopt_t;


IRQ_HANDLER(irq_fdc) {
    printf("\n***FLOPPY***\n");
    irq_fdc_wait = 1;
}

void LBAtoCHS(unsigned long lba, int *cylinder, int *head, int *sector, discopt_t *diskopt) {
    *cylinder = lba / (diskopt->heads_per_cyinder * diskopt->sector_per_track);
    int tmp = lba % (diskopt->heads_per_cyinder * diskopt->sector_per_track);
    *head = tmp / diskopt->sector_per_track;
    *sector = tmp % diskopt->sector_per_track + 1;
}

void fdc_irq_wait(void) {
    irq_fdc_wait = 0;
    while(irq_fdc_wait);
    irq_fdc_wait = 0;
}

char scan_flopies() {
    outportb(0x70, 0x10);
    floppies = inportb(0x71);
    return floppies;
}

int floppy_motor_on(unsigned int drive, unsigned int on) {
    if(drive > 3) return -1;
    char bits;
    if(on) bits = 0xC | drive | (1 << (drive + 4));
    else bits = 0;
    outportb(0x3f2, bits);
    return 0;
}

void floppy_reset(int drive) {
    floppy_motor_on(drive, 1);
    fdc_irq_wait();
    fdc_out(0xF);
    fdc_out(0);
    fdc_out(0);
    inportb(0x3F4);
    outportb(0x3F7, 0);
}

int fdc_out(unsigned char command) {
    int i = 5;
    unsigned char in;
    do {
        in = inportb(0x3F4);
        if(in & 0x80) break;
        timer_tick(5);
    } while(!(in & 0x80) && --i);
    if(!i) return 0;
    outportb(0x3F5, command);
    return 1;
}

int fdc_drive_engene(int drive, int status) {
    if(drive > 4) return 0;
    char out = 1 << (drive + 3);
    out |= 0xC | (drive - 1);
    outportb(0x3F2, out);
    timer_tick(20);
    return 1;
}

int fdc_search_track(int track) {
    if(!fdc_out(0xF)) return 0;
    if(!fdc_out(0x0)) return 0;
    if(!fdc_out(0x12)) return 0;
    fdc_irq_wait();
    return 1;
}
