#include "string.h"
#include "syscall.h"

int sprintf(char *str,char *format,...)
 {
  char buff[12];
  void **params=(void**)&format;
  int param,i,tmp,index=0;
  for(i=0,param=1;format[i]!=0;++i)
   {
    if(format[i]=='%')
     {
      switch(format[++i])
       {
        case 'c':str[index++]=(char)params[param++];
                 break;
        case 's':for(tmp=0;((char*)params[param])[tmp]!=0;++tmp,++index)
                  str[index]=((char*)params[param])[tmp];
                 ++param;
                 break;
        case 'i':itoa((unsigned int)params[param++],buff);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 break;
        case 'x'://Case fall
        case 'X':inttohex((unsigned int)params[param++],buff,4);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 break;
        case 'f':dtoa(*(double*)&params[param++],buff,4);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 ++param;
                 break;
        default:str[index++]=format[i];
       }
     }
    else str[index++]=format[i];
   }
  str[index]=0;
  return index;
 }

int snprintf(char *str,int size,char *format,...)
 {
  char buff[12];
  void **params=(void**)&format;
  int param,i,tmp,index=0;
  for(i=0,param=1;format[i]!=0;++i)
   {
    if(format[i]=='%')
     {
      switch(format[++i])
       {
        case 'c':str[index++]=(char)params[param++];
                 break;
        case 's':for(tmp=0;((char*)params[param])[tmp]!=0;++tmp,++index)
                  str[index]=((char*)params[param])[tmp];
                 ++param;
                 break;
        case 'i':itoa((unsigned int)params[param++],buff);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 break;
        case 'x'://Case fall
        case 'X':inttohex((unsigned int)params[param++],buff,4);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 break;
        case 'f':dtoa(*(double*)&params[param++],buff,4);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 ++param;
                 break;
        default:str[index++]=format[i];
       }
     }
    else str[index++]=format[i];
   }
  str[index]=0;
  return index;
 }

int puts(const char *s)
 {
  int len;
  while(s[len++]);
  write(1,s,len-1);
  return len-1;
 }

void putchar(char c)
 {
  write(1,&c,1);
 }

int printf(const char* format,...)
 {
  char buff[12],str[512];
  void **params=(void**)&format;
  int param,i,tmp,index=0;
  for(i=0,param=1;format[i]!=0;++i)
   {
    if(format[i]=='%')
     {
      switch(format[++i])
       {
        case 'c':str[index++]=(char)params[param++];
                 break;
        case 's':for(tmp=0;((char*)params[param])[tmp]!=0;++tmp,++index)
                  str[index]=((char*)params[param])[tmp];
                 ++param;
                 break;
        case 'i':itoa((unsigned int)params[param++],buff);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 break;
        case 'x'://Case fall
        case 'X':inttohex((unsigned int)params[param++],buff,4);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 break;
        case 'f':dtoa(*(double*)&params[param++],buff,4);
                 for(tmp=0;buff[tmp]!=0;++tmp,++index)str[index]=buff[tmp];
                 ++param;
                 break;
        default:str[index++]=format[i];
       }
     }
    else str[index++]=format[i];
   }
  write(1,str,index);
  return index;
 }
