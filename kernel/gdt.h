#define CODE                    0xC
#define CODE_READABLE           0xD
#define CODE_CONF               0xE
#define CODE_CONF_READABLE      0xF

#define DATA                    0x8
#define DATA_WRITABLE           0x9
#define DATA_STACK              0xA
#define DATA_STACK_WRITABLE     0xB

#define SYS_DESCR               0x08
#define GDT_SIZE                8192

struct _GDTR
 {
  unsigned short limit;
  void *address;
 }__attribute__((packed));
typedef struct _GDTR GDTR;

struct _GDT
 {
  unsigned low_limit:16;   //Low 16 bits of limit
  unsigned low_base:24;    //Low 24 bits of base
  unsigned accessed:1;     //Segment accessed
  unsigned flags:4;
  //unsigned EW:1;         //if CD==0 Writeble 0/1 - Y/N | if CD==1 Readable 0/1 - N/Y
  //unsigned EC:1;         //if CD==0 Expansion direction 1/0 - Stack/default | if CD==1 Conforming
  //unsigned CD:1;         //0/1 - Data/code (if Sys==1)
  //unsigned sys:1;        //0/1 - Segment system/data or code
  unsigned DPL:2;          //Descriptor Privilege Level
  unsigned present:1;      //0/0 - Segment not present/present
  unsigned high_limit:4;   //High 4 bits of limit
  unsigned avl:1;          //Available for system software
  unsigned reserved:1;     //May be 0
  unsigned SB:1;           //if CD==1 0/1 - 16/32 bits code | if CD==0 0/1 - ?/Big
  unsigned granularity:1;  //0/1 - 1 bit/4096 bits
  unsigned high_base:8;    //High 8 bits of base
 }__attribute__((packed));
typedef struct _GDT GDT;

void lgdt(void *address,unsigned short limit); //Loads Global Descriptors Table Registor
//Set GDT item num; If gdt_addr==0 then GDT address take from sgdt; YOU MUST DISABLE INTERRUPTS MANUALY
int set_descriptor(void *gdt_addr,unsigned short num,void *base,unsigned long limit,int present,int DPL,int SB,int granularity,int flags);
//Store GDT item num to store
int get_gdt(int num,GDT *store);
