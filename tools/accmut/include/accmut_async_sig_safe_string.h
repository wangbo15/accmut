#ifndef ACCMUT_STRING_H
#define ACCMUT_STRING_H

unsigned int __accmut__strlen(const char *s){
	unsigned int len = 0;
	while(*s++)
		++len;
	return len;
}

char *__accmut__strcat(char *dest, const char *src){
	char *cp = dest;
	while(*cp)
		++cp;
	while(*cp++ = *src++);
	return dest;
}

char * __accmut__itoa(long n, int base){
	static char buf[36];
	char *p = &buf[36];
	*(--p) = '\0';
	int minus;
	if(n < 0){
		minus = 1;
		n = 0 - n;
	}else
		minus = 0;

	if(n == 0){
		*(--p) = '0';
	}
	else
		while(n > 0){
			*(--p) = "01234567890abcdef"[n % base];
			n /= base;
		}
	if(minus)
		*(--p) = '-';
	return p;
}

#endif