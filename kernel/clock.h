#define RTC_IRQ8_256HZ		0x1	//3,90625ms
#define RTC_IRQ8_128HZ		0x2	//7,8125ms
#define RTC_IRQ8_8192HZ		0x3	//122,070mcs
#define RTC_IRQ8_4094HZ		0x4	//244,141mcs
#define RTC_IRQ8_2048HZ		0x5	//488,281mcs
#define RTC_IRQ8_1024HZ		0x6	//976,5625mcs
#define RTC_IRQ8_512HZ		0x7	//1,953125
//#define RTC_IRQ8_256HZ		0x8	//3,90625
//#define RTC_IRQ8_128HZ		0x9	//7,8125ms
#define RTC_IRQ8_64HZ		0xA	//15,625ms
#define RTC_IRQ8_32HZ		0xB	//31,25ms
#define RTC_IRQ8_16HZ		0xC	//62,5ms
#define RTC_IRQ8_8HZ		0xD	//125ms
#define RTC_IRQ8_4HZ		0xE	//250ms
#define RTC_IRQ8_2HZ		0xF	//500ms

typedef struct _DATETIME
 {
  unsigned short day,month,year,hour,minute,second;
 }DATETIME;


void set_rtc_interval(char interval);
void enable_rtc_interrupt(int enable);
void RTC(void);
void rtc_tick(unsigned long ticks);
void timer_tick(unsigned long ticks);
void irq_timer(void);
void gettime(DATETIME *dt);
