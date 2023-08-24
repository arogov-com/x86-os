// Convert char 'c' to upper case
unsigned char upperchar(unsigned char c);

// Convert char 'c' to lower case
unsigned char lowerchar(unsigned char c);

// Convert NULL terminated string 'str' to upper case
void upperstr(char *str);

// Convert NULL terminated string 'str' to lower case
void lowerstr(char *str);

// Return length of string 'str' without NULL character
int strlen(char *str);

// Copy characters from 'dest' to 'src' and return pointer to 'dest'
char *strcpy(char *dest, char *src);

// Copy 'len' characters from 'dest' to 'src' and return pointer to 'dest'
char *strncpy(char *dest, char *src, int len);

// Concatenate 'str1' and 'str2' and return pointer to 'str1'
char *strcat(char *str1, char *str2);

// Return pointer of character 'c' from string 'str'
char *strchr(char *str, char c);

// Compare 'str1' and 'str2' and return -1 if str1 < str2, 0 if str1 == str2, 1 if str1 > str2
int strcmp(char *str1, char *str2);

// Compare with ignore case 'str1' and 'str2' and return -1 if str1 < str2, 0 if str1 == str2, 1 if str1 > str2
int strcmpi(char *str1, char *str2);

// Find first match of str2 in str1 and return pointer to him
char *strstr(char *str1, char *str2);

// Find with ignore case first match of str2 in str1 and return pointer to him
char *strstri(char *str1, char *str2);

// Fill first 'size' bytes of 'addr'
void *memset(void *addr, int byte, int size);

// Copy 'count' bytes from 'src' to 'dst'
void *memcpy(void *dst, const void *src, unsigned int count);

// Convert 'num' to hexadecimal digit and store to 'dst'
char *itohex(unsigned long num, char *dst, int leading_nuls); //if leading_nuls = 1 and num = 0xABCD (for ex.), out 0x0000ABCD

int itoa(int n, char *str);
int uitoa(unsigned int n, char *str);
unsigned long atoi(const char *str);
void inttohex(unsigned long n, char *buff, int accuraty);
unsigned long hextoint(char *hex, int *ret_status);
void dtoa(double d, char *buff, int accuraty);
