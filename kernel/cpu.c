#include "cpu.h"
#include "ioports.h"

int cpuid(unsigned long *eax, unsigned long *ebx, unsigned long *ecx, unsigned long *edx) {
    asm volatile("cpuid":"=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx):"a"(*eax));
    return 1;
}

void get_processor_vendor(char *str) {
    unsigned long eax = 0, ebx, ecx, edx;
    cpuid(&eax, &ebx, &ecx, &edx);
    *((unsigned long*)str) = ebx;
    *((unsigned long*)(str + 4)) = edx;
    *((unsigned long*)(str + 8)) = ecx;
}

unsigned long get_processor_speed(void) {
    unsigned long long spd;
    unsigned long eax1, edx1, eax2, edx2;
    outportb(0x21, 0xFE);
    asm volatile("hlt");
    asm volatile("rdtsc":"=a"(eax1), "=d"(edx1):);
    asm volatile("hlt");
    asm volatile("rdtsc":"=a"(eax2), "=d"(edx2):);
    outportb(0x21, 0x0);

    spd = ((((unsigned long long)edx2) << 32) | ((unsigned long long)eax2)) - ((((unsigned long long)edx1) << 32) | ((unsigned long long)eax1));

    spd = ((unsigned long)spd) / 18,20648193359375;
    return (unsigned long)spd;
}

unsigned long long rdtsc(void) {
  unsigned long eax, edx;
  asm volatile("rdtsc":"=a"(eax), "=d"(edx):);
  return ((((unsigned long long)edx) << 32) | ((unsigned long long)eax));
}
