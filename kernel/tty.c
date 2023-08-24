#include "tty.h"
#include "keyboard.h"
#include "scancodes.h"
#include "virtualkey.h"
#include "ioports.h"

int tty_cursor;
char tty_attribute;
void *video_ram;

void init_tty(void *video_ram_addr)
 {
  video_ram=video_ram_addr?video_ram_addr:VIDEO_RAM_DEFAULT;
  tty_cursor=0;
  outportb(0x3D4,0xF);
  outportb(0x3D5,(unsigned char)tty_cursor);
  outportb(0x3D4,0xE);
  outportb(0x3D5,(unsigned char)(tty_cursor>>8));
  tty_attribute=7;
 }
void set_video_ram_addr(void *video_ram_addr)
 {
  video_ram=video_ram_addr;
 }
void setattribute(char c)
 {
  tty_attribute=c;
 }

char getattribute(void)
 {
  return tty_attribute;
 }

void getxy(unsigned short *x,unsigned short *y)
 {
  *y=tty_cursor/VIDEO_WIDTH;
  *x=tty_cursor-((*y)*VIDEO_WIDTH);
 }

void gotoxy(unsigned short x,unsigned short y)
 {
  tty_cursor=(int)((y*VIDEO_WIDTH)+x);
  outportb(0x3D4,0xF);
  outportb(0x3D5,(unsigned char)tty_cursor);
  outportb(0x3D4,0xE);
  outportb(0x3D5,(unsigned char)(tty_cursor>>8));
 }

void cursor_enable(int enabled)
 {
  asm("movb $0xA,%al");
  asm("movw $0x3D4,%dx");
  asm("outb %al,%dx");
  if(enabled)asm("movb $0xB,%al");
  else asm("movb $0x20,%al");
  asm("movw $0x3D5,%dx");
  asm("outb %al,%dx");
 }

void unputchar(void)
 {
  char *video=(char*)video_ram;
  --tty_cursor;
  *(video + tty_cursor*2)=' ';
  *(video + tty_cursor*2+1)=tty_attribute;
  outportb(0x3D4,0xF);
  outportb(0x3D5,(unsigned char)tty_cursor);
  outportb(0x3D4,0xE);
  outportb(0x3D5,(unsigned char)(tty_cursor>>8));
 }

char getch(void)
 {
  unsigned short scancode;
  unsigned char creg;
  static unsigned char shift=0;

  while(1)
   {
    scancode=readkey();
    if(scancode>=0x47 && scancode<=0x53 && get_num_state())
     {
      switch(scancode)
       {
        case VK_NUM_7:return scancodes[0x8];break;
        case VK_NUM_8:return scancodes[0x9];break;
        case VK_NUM_9:return scancodes[0xA];break;
        case VK_NUM_4:return scancodes[0x5];break;
        case VK_NUM_5:return scancodes[0x6];break;
        case VK_NUM_6:return scancodes[0x7];break;
        case VK_NUM_1:return scancodes[0x2];break;
        case VK_NUM_2:return scancodes[0x3];break;
        case VK_NUM_3:return scancodes[0x4];break;
        case VK_NUM_0:return scancodes[0xB];break;
        case VK_NUM_DOT:return scancodes[0x34];break;
       }
     }
    if(scancode==VK_SHIFT || scancode==VK_RSHIFT)shift=1;
    else if(scancode==VK_SHIFT+KEY_UP || scancode==VK_RSHIFT+KEY_UP)shift=0;
    else if(scancode<KEY_UP)
     {
      if(shift)return scancodes_shifted[scancode];
      else return scancodes[scancode];
     }
   }
 }

int gets(char *str)
 {
  int i=0;
  int x,y;
  char c;
  while((c=getch())!=0xA)
   {
    if(c==0x8)
     {
      if(i)
       {
        --i;
        unputchar();
       }
     }
    else
     {
      if(c)
      {
       str[i++]=c;
       putchar(c);
      }
     }
   }
  putchar('\n');
  str[i]=0;
  return i;
 }
