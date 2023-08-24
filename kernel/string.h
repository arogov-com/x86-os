unsigned char upperchar(unsigned char c);
unsigned char lowerchar(unsigned char c);

void upperstr(char *str);
void lowerstr(char *str);

int strlen(char *str);
char *strcpy(char *dest,char *src);
char *strncpy(char *dest,char *src,int len);
char *strcat(char *str1,char *str2);
char *strchr(char *str,char c);

int strcmp(char *str1,char *str2);
int strcmpi(char *str1,char *str2);

char *strstr(char *str1,char *str2);
char *strstri(char *str1,char *str2);

void memset(void *addr,char byte,int size);
void memcpy(void *dst,void *src,int size);

char *itohex(unsigned long num,char *dst,int leading_nuls); //if leading_nuls=1 and num=0xABCD (for ex.), out 0x0000ABCD
int itoa(unsigned long n,char *str);
unsigned long atoi(const char *str);
void inttohex(unsigned long n,char *buff,int accuraty);
unsigned long hextoint(char *hex,int *ret_status);
void dtoa(double d,char *buff,int accuraty);
