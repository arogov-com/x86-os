int cpuid(unsigned long *eax,unsigned long *ebx,unsigned long *ecx,unsigned long *edx);
void get_processor_vendor(char *str);
unsigned long get_processor_speed(void);
unsigned long long rdtsc(void);
