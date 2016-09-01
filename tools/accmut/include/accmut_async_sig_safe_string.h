#ifndef ACCMUT_STRING_H
#define ACCMUT_STRING_H

unsigned int __accmut__strlen(const char *s);

char *__accmut__strcat(char *dest, const char *src);

char * __accmut__itoa(long n, int base);

#endif
