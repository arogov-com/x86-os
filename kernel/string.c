// Copyright (C) 2023 Aleksei Rogov <alekzzzr@gmail.com>. All rights reserved.

#include <stddef.h>
#include "string.h"

unsigned char upperchar(unsigned char c) {
    return (c >= 'a' && c <= 'z') ? (c - 0x20) : (c);
}

unsigned char lowerchar(unsigned char c) {
    return (c >= 'A' && c <= 'Z') ? (c + 0x20) : (c);
}

void upperstr(char *str) {
    if(!str) return;
    while(*str) *str = upperchar(*str), ++str;
}

void lowerstr(char *str) {
    if(!str) return;
    while(*str) *str = upperchar(*str), ++str;
}

int strlen(char *str) {
    if(!str) return -1;
    char *s = str;
    while(*(s++));
    return (s - str - 1);
}

char *strcpy(char *dest, char *src) {
    if(!dest || !src) return NULL;
    char *ret = dest;
    while(*(dest++) = *(src++));
    return ret;
}

char *strncpy(char *dest, char *src, int len) {
    if(!dest || !src) return NULL;
    char *ret = dest;
    dest[len] = 0;
    while(len-- && (*(dest++) = *(src++)));
    return ret;
}

char *strcat(char *str1, char *str2) {
    if(!str1 || !str2) return NULL;
    char *ret = str1;
    while(*(str1++));
    --str1;
    while(*(str2)) *(str1++) = *(str2++);
    *str1 = 0;
    return ret;
}

char *strchr(char *str, char c) {
    if(!str) return NULL;
    while(*str != 0 && *(str++) != c);
    if(!*str) return (char*)0;
    return --str;
}

int strcmp(char *str1, char *str2) {
    if(!str1 || !str2) return 0;
    for(; *str1 == *str2; ++str1, ++str2)
        if(*str1 == '\0') return 0;
    return (*str1 - *str2);
}

int strcmpi(char *str1, char *str2) {
    if(!str1 || !str2) return 0;
    for(; upperchar(*str1) == upperchar(*str2); ++str1, ++str2)
        if(*str1 == '\0') return 0;
    return (*str1 - *str2);
}

char *strstr(char *str1, char *str2) {
    if(!str1 || !str2) return NULL;
    int f;
    char *str3 = str2, *strr = 0;
    while(*str1) {
        if(!*str3) return f ? (strr) : (0);
        if(*str1++ == *str3++ && (f = 1)) strr = strr ? strr : str1 - 1;
        else f = 0, strr = 0, str3 = str2;
    }
    return 0;
}

char *strstri(char *str1, char *str2) {
    if(!str1 || !str2) return NULL;
    int f;
    char *str3 = str2, *strr = 0;
    while(*str1) {
        if(!*str3) return f ? (strr) : (0);
        if(upperchar(*str1++) == upperchar(*str3++) && (f = 1)) strr = strr ? strr : str1 - 1;
        else f = 0, strr = 0, str3 = str2;
    }
    return 0;
}

void *memset(void *addr, int byte, int size) {
    register unsigned int bloks = size >> 2;
    byte = ((byte << 24) & 0xFF000000) | ((byte << 16) & 0xFF0000) | ((byte << 8) & 0xFF00) | (byte & 0xFF);
    while(bloks--) {
        *((unsigned int*)addr) = byte;
        addr = (void*)((unsigned int)addr + 4);
    }
    bloks = size & 3;
    while(bloks--) {
        *((unsigned char*)addr) = byte;
        addr = (void*)((unsigned int)addr + 1);
    }
    return (void*)((unsigned int)addr - size);
}

void *memcpy(void *dst, const void *src, unsigned int count) {
    register unsigned int cntb = count >> 2;
    while(cntb--) {
        *((unsigned int*)dst) = *((unsigned int*)src);
        dst = (void*)((unsigned int)dst + 4);
        src = (void*)((unsigned int)src + 4);
    }
    cntb = count & 3;
    while(cntb--) {
        *((unsigned char*)dst) = *((unsigned char*)src);
        dst = (void*)((unsigned int)dst + 1);
        src = (void*)((unsigned int)src + 1);
    }
    return (void*)((unsigned int)dst - count);
}
/*
void memset(void *addr,char byte,int size)
 {
  unsigned long dw=(((unsigned long)byte<<24)&0xFF000000)|(((unsigned long)byte<<16)&0xFF0000)|(((unsigned long)byte<<8)&0xFF00)|(byte&0xFF);
  int dwords=size>>2;
  int i;
  if(dwords)
   for(i=0;i!=dwords;++i)
    *((unsigned long*)addr+i)=dw;
  for(i=i<<2;i!=size;++i)
   *((unsigned char*)addr+i)=byte;
 }
void memcpy(void *dst,void *src,int size)
 {
  int dwords=size>>2;
  int i;
  if(dwords)
   for(i=0;i!=dwords;++i)
    *((unsigned long*)dst+i)=*((unsigned long*)src+i);
  for(i=i<<2;i!=size;++i)
   *((unsigned char*)dst+i)=*((unsigned char*)src+i);
 }
*/

char *itohex(unsigned long num, char *dst, int leading_nuls) {
    unsigned long d, pos;
    long shift;
    if(num > 0xFFFF) shift = 28;
    else if(num > 0xFF) shift = 12;
    else shift = 4;
    for(pos = 0; shift >= 0; shift -= 4) {
        d = num;
        d = (d >> shift) & 0xF;
        dst[pos] = d + '0';
        if(d >= 10 && d <= 0xF) dst[pos] += 7;
        if(!d && !leading_nuls) continue;
        leading_nuls = 1;
        ++pos;
    }
    if(!num && !leading_nuls) dst[pos++] = '0';
    dst[pos] = 0;
    return dst;
}

int itoa(int n, char *str) {
    int sign = 0, i = 0, tmp;
    if(!str) return 0;
    if(n<0) sign = 1, n = -n;
    do {
        tmp = n % 10;
        str[i++] = tmp + '0';
        n /= 10;
    }while(n);
    if(sign) str[i++] = '-';
    str[i--] = 0;
    tmp = 0;
    while(tmp < i) {
      n = str[tmp];
      str[tmp++] = str[i];
      str[i--] = n;
    }
    return tmp + i + 1;
}

int uitoa(unsigned int n, char *str) {
    unsigned int i = 0, tmp;
    if(!str) return 0;
    do {
        tmp = n % 10;
        str[i++] = tmp + '0';
        n /= 10;
    }while(n);
    str[i--] = 0;
    tmp = 0;
    while(tmp < i) {
        n = str[tmp];
        str[tmp++] = str[i];
        str[i--] = n;
    }
    return tmp + i + 1;
}

unsigned long atoi(const char *str) {
    if(!str) return -1;
    unsigned long n = 0;
    int i = 0;
    do {
        n *= 10;
        n += (str[i] - '0');
    } while(str[++i]);
    return n;
}

void inttohex(unsigned long n, char *buff, int accuraty) {
    unsigned long mask = 0xF0000000, tmp;
    unsigned int i = -1;
    char shift = 28;
    do {
        tmp = n & mask;
        tmp = tmp >> shift;
        mask = mask >> 4;
        shift -= 4;
        if(accuraty <= (shift + 4) / 8) continue;
        buff[++i] = tmp + '0';
        if(tmp > 9) buff[i] += 7;
    }while(shift > -4);
    buff[++i] = 0;
}

unsigned long hextoint(char *hex, int *ret_status) {
    unsigned long ret = 0;
    if(hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) hex += 2;
    int i;
    for(i = 0; hex[i] != 0; ++i) {
        ret = ret << 4;
        if(hex[i] >= '0' && hex[i] <= '9')
            ret += hex[i] - '0';
        else if(hex[i] >= 'a' && hex[i] <= 'f')
            ret += hex[i] - '0' - 39;
        else if(hex[i] >= 'A' && hex[i] <= 'F')
            ret += hex[i] - '0' - 7;
        else if(hex[i] == 'h' || hex[i] == 'H') break;
        else {
            ret_status ? (*ret_status = 0) : (0);
            return 0;
        }
    }
    ret_status ? (*ret_status = 1) : (0);
    return ret;
}

void dtoa(double d, char *buff, int accuraty) {
    int i = 0;
    buff[i++] = (int)d + '0';
    d = (d - (int)d) * 10.0;
    buff[i++] = '.';
    do {
        buff[i++] = (int)d + '0';
        d = (d - (int)d) * 10.0;
    }while(--accuraty);
    buff[i] = 0;
 }
