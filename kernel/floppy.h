void irq_fdc(void);
void fdc_irq_wait(void);
char scan_flopies();
int floppy_motor_on(unsigned int drive, unsigned int on);
void floppy_reset(int drive);

int fdc_drive_engene(int drive, int status);
int fdc_out(unsigned char command);
int fdc_search_track(int track);
