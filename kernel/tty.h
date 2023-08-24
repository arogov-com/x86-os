#define VIDEO_RAM_DEFAULT 0xB8000
#define VIDEO_WIDTH 80
#define VIDEO_HEIGHT 25

#define FOREGROUND_BLACK      0x0
#define FOREGROUND_BLUE       0x1
#define FOREGROUND_GREEN      0x2
#define FOREGROUND_LIGHT_BLUE 0x3
#define FOREGROUND_RED        0x4
#define FOREGROUND_PURPLE     0x5
#define FOREGROUND_BROWN      0x6
#define FOREGROUND_GRAY       0x7

#define BACKGROUND_BLUE       0x10
#define BACKGROUND_GREEN      0x20
#define BACKGROUND_LIGHT_BLUE 0x30
#define BACKGROUND_RED        0x40
#define BACKGROUND_PURPLE     0x50
#define BACKGROUND_BROWN      0x60
#define BACKGROUND_GRAY       0x70

#define LIGHT 0x8
#define BLINK 0x80

void init_tty(void *video_ram_addr);
void set_video_ram_addr(void *video_ram_addr);

void setattribute(char c);
char getattribute(void);

void getxy(unsigned short *x,unsigned short *y);
void gotoxy(unsigned short x,unsigned short y);

void cursor_enable(int enabled);

void unputchar(void);

char getch(void);
int gets(char *str);
