#include "string.h"
#include "syscall.h"
#include "keyboard.h"
#include "virtualkey.h"
#include "scancodes.h"

int sprintf(char *str, char *format, ...) {
    char buff[12];
    void **params = (void**)&format;
    int param, i, tmp, index = 0;
    for(i = 0, param = 1; format[i] != 0; ++i) {
        if(format[i] == '%') {
            switch(format[++i]) {
                case 'c': str[index++] = (char)params[param++];
                          break;
                case 's': for(tmp = 0; ((char*)params[param])[tmp] != 0; ++tmp, ++index)
                              str[index]=((char*)params[param])[tmp];
                          ++param;
                          break;
                case 'i': itoa((unsigned int)params[param++], buff);
                          for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                          break;
                case 'x': //Case fall
                case 'X': inttohex((unsigned int)params[param++], buff, 4);
                          for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                          break;
                case 'f': dtoa(*(double*)&params[param++], buff, 4);
                          for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                          ++param;
                          break;
                default: str[index++] = format[i];
            }
        }
        else str[index++] = format[i];
    }
    str[index] = 0;
    return index;
}

int snprintf(char *str, int size, char *format, ...) {
    char buff[12];
    void **params = (void**)&format;
    int param, i, tmp, index = 0;
    for(i = 0, param = 1; format[i] != 0; ++i) {
        if(format[i] == '%') {
            switch(format[++i]) {
                case 'c': str[index++] = (char)params[param++];
                          break;
                case 's': for(tmp = 0; ((char*)params[param])[tmp] != 0; ++tmp, ++index)
                              str[index] = ((char*)params[param])[tmp];
                          ++param;
                          break;
                case 'i': itoa((unsigned int)params[param++], buff);
                          for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                          break;
                case 'x': //Case fall
                case 'X': inttohex((unsigned int)params[param++], buff, 4);
                          for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                          break;
                case 'f': dtoa(*(double*)&params[param++], buff, 4);
                          for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                          ++param;
                          break;
                default: str[index++] = format[i];
            }
        }
        else str[index++] = format[i];
    }
    str[index] = 0;
    return index;
}

int puts(const char *s) {
    int len = 0;
    while(s[len++]);
    write(1, s, len - 1);
    return len - 1;
}

void putchar(char c) {
      write(1, &c, 1);
}

char getch(void) {
    unsigned short scancode;
    static unsigned char shift = 0;

    while(1) {
        read(0, &scancode, 1);
        if(scancode >= 0x47 && scancode <= 0x53 && get_num_state()) {
            switch(scancode) {
                case VK_NUM_7: return scancodes[0x8]; break;
                case VK_NUM_8: return scancodes[0x9]; break;
                case VK_NUM_9: return scancodes[0xA]; break;
                case VK_NUM_4: return scancodes[0x5]; break;
                case VK_NUM_5: return scancodes[0x6]; break;
                case VK_NUM_6: return scancodes[0x7]; break;
                case VK_NUM_1: return scancodes[0x2]; break;
                case VK_NUM_2: return scancodes[0x3]; break;
                case VK_NUM_3: return scancodes[0x4]; break;
                case VK_NUM_0: return scancodes[0xB]; break;
                case VK_NUM_DOT: return scancodes[0x34]; break;
            }
        }
        if(scancode == VK_SHIFT || scancode == VK_RSHIFT) {
            shift = 1;
        }
        else if(scancode == VK_SHIFT + KEY_UP || scancode == VK_RSHIFT + KEY_UP) {
            shift = 0;
        }
        else if(scancode < KEY_UP) {
            if(shift) {
                return scancodes_shifted[scancode];
            }
            else {
                return scancodes[scancode];
            }
        }
    }
}

int gets(char *str) {
    int i = 0;
    char c;
    while((c = getch()) != 0xA) {
        if(c == 0x8) {
            if(i) {
                --i;
                unputchar();
            }
        }
        else {
            if(c) {
                str[i++] = c;
                putchar(c);
            }
        }
    }
    putchar('\n');
    str[i] = 0;
    return i;
}

int printf(const char* format, ...) {
    char buff[12], str[512];
    void **params = (void**)&format;
    int param, i, tmp, index = 0;
    for(i = 0, param = 1; format[i] != 0; ++i) {
        if(format[i] == '%') {
            switch(format[++i]) {
                case 'c': str[index++] = (char)params[param++];
                        break;
                case 's': for(tmp = 0;((char*)params[param])[tmp] != 0; ++tmp, ++index)
                            str[index]=((char*)params[param])[tmp];
                        ++param;
                        break;
                case 'i': itoa((unsigned int)params[param++], buff);
                        for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                        break;
                case 'x': //Case fall
                case 'X': inttohex((unsigned int)params[param++], buff, 4);
                        for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                        break;
                case 'f': dtoa(*(double*)&params[param++], buff, 4);
                        for(tmp = 0; buff[tmp] != 0; ++tmp, ++index) str[index] = buff[tmp];
                        ++param;
                        break;
                default: str[index++] = format[i];
            }
        }
        else str[index++] = format[i];
    }
    write(1, str, index);
    return index;
}
