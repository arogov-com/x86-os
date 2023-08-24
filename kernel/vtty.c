#include "vtty.h"
#include "ioports.h"
#include "virtualkey.h"

VTTY vtty[12];

char ctrl,rctrl;
char alt,ralt;
char shift,rshift;

void init_vttys(void)
 {
  ctrl=0;rctrl=0;
  alt=0;ralt=0;
  shift=0;rshift=0;

  memset(&vtty,0,sizeof(VTTY)*12);
 }

int mk_vtty(int number,void *video,void *read_buffer,int read_buffer_max)
 {
  if(number>11 || video<0x10000 || read_buffer<0x10000)return 0;
  vtty[number].cursor=0;
  vtty[number].attribute=7;
  vtty[number].read_buffer_max=read_buffer_max;
  vtty[number].video=video;
  vtty[number].read_buffer=read_buffer;
  memset(video,0,4096);
  memset(read_buffer,0,read_buffer_max);    
  return 1;
 }

int get_active_vtty(void)
 {
  int i;
  for(i=0;i!=12;++i)if(vtty[i].active)return i;
  return -1;
 }

int set_active_vtty(int number)
 {
  if(number>11)return 0;
  int active=get_active_vtty();
  if(active!=-1)vtty[active].active=0;
  vtty[number].active=1;
  return 1;
 }

int set_vtty_attribute(unsigned char attribute)
 {
  int active=get_active_vtty();
  if(active==-1)return 0;
  vtty[active].attribute=attribute;
  return 1;
 }
int set_vtty_attributeex(unsigned char attribute,unsigned int vtty_num)
 {
  if(vtty_num>11)return 0;
  vtty[vtty_num].attribute=attribute;
  return 1;
 }
unsigned char get_vtty_attribute(void)
 {
  int active=get_active_vtty();
  if(active==-1)return -1;
  return vtty[active].attribute;
 }
unsigned char get_vtty_attributeex(unsigned int vtty_num)
 {
  if(vtty_num>11)return -1;
  return vtty[vtty_num].attribute;
 }

int switch_vtty(int newvtty)
 {
  if(newvtty>11)return 0;
  int current_tty=get_active_vtty();
  if(current_tty==-1)
   {
    vtty[newvtty].active=1;
    return 1;
   }
  if(current_tty==newvtty)return 1;

  memcpy(vtty[current_tty].video,VIDEO_RAM_DEFAULT,4096);
  memcpy(VIDEO_RAM_DEFAULT,vtty[newvtty].video,4096);

  vtty[current_tty].active=0;
  vtty[newvtty].active=1;
  
  outportb(0x3D4,0xF);
  outportb(0x3D5,(unsigned char)vtty[newvtty].cursor);
  outportb(0x3D4,0xE);
  outportb(0x3D5,(unsigned char)(vtty[newvtty].cursor>>8));

  return 1;
 }

int clrscr(void)
 {
  int active=get_active_vtty();
  if(active==-1)return 0;
  int i;
  for(i=0;i<VIDEO_HEIGHT*VIDEO_WIDTH;i++)*(char*)(VIDEO_RAM_DEFAULT+i*2)=' ';
  vtty[active].cursor=0;
  outportb(0x3D4,0xF);
  outportb(0x3D5,(unsigned char)vtty[active].cursor);
  outportb(0x3D4,0xE);
  outportb(0x3D5,(unsigned char)(vtty[active].cursor>>8));
  return 1;
 }

void vtty_cursor_enable(int enabled)
 {
  asm("movb $0xA,%al");
  asm("movw $0x3D4,%dx");
  asm("outb %al,%dx");
  if(enabled)asm("movb $0xB,%al");
  else asm("movb $0x20,%al");
  asm("movw $0x3D5,%dx");
  asm("outb %al,%dx");
 }

int vtty_getxy(unsigned short *x,unsigned short *y)
 {
  int active=get_active_vtty();
  if(active==-1)return 0;
  *x=vtty[active].cursor%VIDEO_WIDTH;
  *y=vtty[active].cursor/VIDEO_WIDTH;
  return 1;
 }

int vtty_gotoxy(unsigned short x,unsigned short y)
 {
  int active=get_active_vtty();
  if(active==-1)return 0;
  vtty[active].cursor=(int)((y*VIDEO_WIDTH)+x);
  outportb(0x3D4,0xF);
  outportb(0x3D5,(unsigned char)vtty[active].cursor);
  outportb(0x3D4,0xE);
  outportb(0x3D5,(unsigned char)(vtty[active].cursor>>8));
  return 1;
 }

int vtty_write(unsigned int vttynum,char *buff,int length)
 {
  int active=get_active_vtty();
  if(vttynum>11 && vttynum!=-1)return -1;
  if(vttynum==-1)vttynum=active;
  if(!vtty[vttynum].video)return -1;

  int i,j;
  for(i=0;i!=length;++i)
   {
    switch(buff[i])
     {
      case '\n':vtty[vttynum].cursor+=VIDEO_WIDTH;
                vtty[vttynum].cursor-=vtty[vttynum].cursor%VIDEO_WIDTH;
                break;
      default:if(active!=vttynum)
               {
                *(char*)(vtty[vttynum].video+vtty[vttynum].cursor*2)=buff[i];
                *(char*)(vtty[vttynum].video+vtty[vttynum].cursor*2+1)=vtty[vttynum].attribute;
                *(char*)(vtty[vttynum].video+vtty[vttynum].cursor*2+3)=vtty[vttynum].attribute;
                ++vtty[vttynum].cursor;
               }
              else
               {
                *(char*)(VIDEO_RAM_DEFAULT+vtty[vttynum].cursor*2)=buff[i];
                *(char*)(VIDEO_RAM_DEFAULT+vtty[vttynum].cursor*2+1)=vtty[vttynum].attribute;
                *(char*)(VIDEO_RAM_DEFAULT+vtty[vttynum].cursor*2+3)=vtty[vttynum].attribute;
                ++vtty[vttynum].cursor;
               }
     }
    if(vtty[vttynum].cursor>VIDEO_WIDTH*VIDEO_HEIGHT)
     {
      if(active!=vttynum)
       for(j=VIDEO_WIDTH*2;j<=VIDEO_WIDTH*VIDEO_HEIGHT*2+VIDEO_WIDTH*2;j++)
        *(char*)(vtty[vttynum].video+j-VIDEO_WIDTH*2)=*(char*)(vtty[vttynum].video+j);
      else
       for(j=VIDEO_WIDTH*2;j<=VIDEO_WIDTH*VIDEO_HEIGHT*2+VIDEO_WIDTH*2;j++)
        *(char*)(VIDEO_RAM_DEFAULT+j-VIDEO_WIDTH*2)=*(char*)(VIDEO_RAM_DEFAULT+j);
      vtty[vttynum].cursor-=VIDEO_WIDTH;
     }
   }
  if(active==vttynum)
   {
    outportb(0x3D4,0xF);
    outportb(0x3D5,(unsigned char)vtty[vttynum].cursor);
    outportb(0x3D4,0xE);
    outportb(0x3D5,(unsigned char)(vtty[vttynum].cursor>>8));
   }
  return length;
 }

unsigned char vtty_read(void)
 {
  unsigned short sc;
  
 }

void vtty_handler(unsigned short scancode)
 {
  if(scancode==VK_CTRL)ctrl=1;
  else if(scancode==VK_RCTRL)rctrl=1;
  else if(scancode==VK_ALT)alt=1;
  else if(scancode==VK_RALT)ralt=1;

  if(scancode==(VK_CTRL | KEY_UP))ctrl=0;
  else if(scancode==(VK_RCTRL | KEY_UP))rctrl=0;
  else if(scancode==(VK_ALT | KEY_UP))alt=0;
  else if(scancode==(VK_RALT | KEY_UP))ralt=0;

  if(alt || ralt)
   {
    switch(scancode)
     {
      case VK_F1:switch_vtty(0);break;
      case VK_F2:switch_vtty(1);break;
      case VK_F3:switch_vtty(2);break;
      case VK_F4:switch_vtty(3);break;
      case VK_F5:switch_vtty(4);break;
      case VK_F6:switch_vtty(5);break;
      case VK_F7:switch_vtty(6);break;
      case VK_F8:switch_vtty(7);break;
      case VK_F9:switch_vtty(8);break;
      case VK_F10:switch_vtty(9);break;
     }
   }
 }
