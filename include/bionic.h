#ifndef __BIONIC_H__
#define __BIONIC_H__
#   define ULONG_MAX	4294967295UL
size_t _strspn(const char *s1, const char *s2);
char * _strpbrk(const char *s1, const char *s2);
char *_strtok(char *str, const char *delim, char **saveptr);
int _strcasecmp(const char *s1, const char *s2);
void *_memcpy(void *dst0, const void *src0, size_t length);
void _memset(void *dst0, char val, size_t length);
unsigned long _strtoul(const void *nptr, void **endptr, int base);
#endif /* __BIONIC_H__ */
