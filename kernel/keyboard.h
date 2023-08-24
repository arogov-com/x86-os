#define REP_WAIT_250   0x0
#define REP_WAIT_500   0x20
#define REP_WAIT_750   0x40
#define REP_WAIT_1000  0x60

#define REP_SPEED_30     0x0
#define REP_SPEED_24     0x2
#define REP_SPEED_20     0x4
#define REP_SPEED_16     0x7
#define REP_SPEED_15     0x8
#define REP_SPEED_12     0xA
#define REP_SPEED_10     0xC
#define REP_SPEED_8      0xF
#define REP_SPEED_6      0x12
#define REP_SPEED_5      0x14
#define REP_SPEED_4      0x17
#define REP_SPEED_3      0x1A
#define REP_SPEED_2      0x1F

#define LED_SCROLL       0x1
#define LED_NUM          0x2
#define LED_CAPS         0x4

void keyboard_init();
void irq_keyboard(void);
void keyboard_wait(void);
unsigned short readkey(void);
void set_autorepeat(char opt);
void set_led(int led, int state);
int get_num_state(void);
int get_caps_state(void);
int get_scroll_state(void);
