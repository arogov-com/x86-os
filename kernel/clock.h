#define RTC_IRQ8_256HZ		0x1	//3,90625ms
#define RTC_IRQ8_128HZ		0x2	//7,8125ms
#define RTC_IRQ8_8192HZ		0x3	//122,070us
#define RTC_IRQ8_4094HZ		0x4	//244,141us
#define RTC_IRQ8_2048HZ		0x5	//488,281us
#define RTC_IRQ8_1024HZ		0x6	//976,5625us
#define RTC_IRQ8_512HZ		0x7	//1,953125 ms
//#define RTC_IRQ8_256HZ		0x8	//3,90625
//#define RTC_IRQ8_128HZ		0x9	//7,8125ms
#define RTC_IRQ8_64HZ		0xA	//15,625ms
#define RTC_IRQ8_32HZ		0xB	//31,25ms
#define RTC_IRQ8_16HZ		0xC	//62,5ms
#define RTC_IRQ8_8HZ		0xD	//125ms
#define RTC_IRQ8_4HZ		0xE	//250ms
#define RTC_IRQ8_2HZ		0xF	//500ms

#define PIT_FREQ            1193180
#define PIT_CH0_PORT        0x40
#define PIT_CH1_PORT        0x41
#define PIT_CH2_PORT        0x42
#define PIT_CMD_PORT        0x43

#define PIT_CH0_CMD         0x0
#define PIT_CH1_CMD         0x40
#define PIT_CH2_CMD         0x80
#define PIT_ACC_LATCH       0x00   // Latch count value command
#define PIT_ACC_LO          0x10   // Access mode: lobyte only
#define PIT_ACC_HI          0x20   // Access mode: hibyte only
#define PIT_ACC_HILO        0x30   // Access mode: lobyte/hibyte
#define PIT_MODE0           0x0    // interrupt on terminal count
#define PIT_MODE1           0x2    // hardware re-triggerable one-shot
#define PIT_MODE2           0x4    // rate generator
#define PIT_MODE3           0x6    // square wave generator
#define PIT_MODE4           0x8    // software triggered strobe
#define PIT_MODE5           0xa    // hardware triggered strobe
#define PIT_BIN             0x0
#define PIT_BCD             0x1

typedef struct {
    unsigned short day, month, year, hour, minute, second;
}datetime_t;

void set_i8254_counter(unsigned short counter);
void set_rtc_interval(char interval);
void enable_rtc_interrupt(int enable);
void RTC(void);
void rtc_tick(unsigned long ticks);
void timer_tick(unsigned long ticks);
void irq_timer(void);
void gettime(datetime_t *dt);
void set_i8254_freq(unsigned short freq);
