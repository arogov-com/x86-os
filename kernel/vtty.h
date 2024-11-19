// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#define VIDEO_RAM_DEFAULT     0xB8000
#define VIDEO_WIDTH           80
#define VIDEO_HEIGHT          25

#define FOREGROUND_BLACK      0x0
#define FOREGROUND_BLUE       0x1
#define FOREGROUND_GREEN      0x2
#define FOREGROUND_CYAN       0x3
#define FOREGROUND_RED        0x4
#define FOREGROUND_MAGENTA    0x5
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

typedef struct _VTTY {
    int active;                          // TTY is active
    int cursor;                          // Cursor position
    char attribute;                      // Attribute byte
    unsigned int read_buffer_max;        // Maximum bytes to read
    char *video;                         // Screen image
    char *read_buffer;                   // Reading buffer
}VTTY;

//Init start keys state and fill by 0x0 VTTY structure
void init_vttys(void);

//Create vtty
int mk_vtty(int number, void *video, void *read_buffer, int read_buffer_max);

int clrscr(void);

void vtty_cursor_enable(int enabled);

int get_active_vtty(void);
int set_active_vtty(int number);

int set_vtty_attribute(unsigned char attribute);
unsigned char get_vtty_attribute(void);
int set_vtty_attributeex(unsigned char attribute, unsigned int vtty_num);
unsigned char get_vtty_attributeex(unsigned int vtty_num);


int switch_vtty(int newvtty);

int vtty_getxy(unsigned short *x, unsigned short *y);
int vtty_gotoxy(unsigned short x, unsigned short y);

int vtty_write(unsigned int vttynum, const char *buff, int length);

int vtty_read(void *buff, int count);

void vtty_handler(unsigned short scancode);
int set_vtty_ram_addr(void *addr);
int set_vtty_ram_addrex(void *addr, unsigned int vtty_num);
