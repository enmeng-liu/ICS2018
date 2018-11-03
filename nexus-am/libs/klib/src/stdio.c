#include "klib.h"
#include <stdarg.h>

//#ifndef __ISA_NATIVE__
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


void _puts(char* s){
	for(int i = 0; i < strlen(s); ++i) _putc(s[i]);
}

char* itoa(char* dest, int n){
	/*transfer int into char[]*/
	//int outn = n;
	if(n == 0) {
		dest[0] = '0';
		dest[1] = '\0';
		return dest;
	}
	
	int len = 0;
	int neg = 0;
	if(n < 0) {
		neg = 1;
		len ++;
		n = -n;
	}
	int temp = n;
	while(temp > 0){
		len ++;
		temp /= 10;
	}
	
	for(int i = 0; i < len; ++i){
		int x = n % 10;
		dest[len - i - 1] = x + '0';
		//printf("dest[%d] = %c\n", len-i-1,dest[len-i-1]);
		n /= 10;
	}
	if(neg) dest[0] = '-';
	dest[len] = '\0';
	//printf("%d -> %s\n",outn,dest);
	return dest;
}

int printf(const char *format, ...) {
	int len = strlen(format);
	int i = 0;
	int in_size = 0;
	va_list var_arg;
	va_start(var_arg, format);

	while(i < len){
		if(format[i] == '%'){
			int in_num = 0;
			char in_char[50] = "";
			switch(format[i+1]){
				case 'd'://%d 
					in_num = va_arg(var_arg,int);
					itoa(in_char,in_num);
					//strcat(str, in_char);
					_puts(in_char);
					in_size += strlen(in_char);
					i += 2;
					break;
				case 's'://%s
				 	strcpy(in_char,va_arg(var_arg, char*));
					//strcat(str,in_char);
					_puts(in_char);
					in_size += strlen(in_char);
					i += 2;
					break;
				case '0'://%02d
					in_num = va_arg(var_arg,int);
					itoa(in_char, in_num);
					int pwidth = format[i+2] - '0';//width of number
					for(int i = strlen(in_char); i < pwidth; ++i) {
						in_size ++;
						_putc('0');
					}
					_puts(in_char);
					in_size += strlen(in_char);
					i += 4;
					break;

				default: printf("Not implememted argument type!\n");
			}
		}
		else {
			//char temp[2];
			//temp[0] = format[i];
			//temp[1] = '\0';
			//strcat(str, temp);
			_putc(format[i]);
			in_size ++;
			i ++;
		}
	}
		va_end(var_arg);
		return in_size;
}
int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}


int sprintf(char* str, const char* format, ...){
	int len = strlen(format);
	int i = 0;
	int in_size = 0;
	va_list var_arg;
	va_start(var_arg, format);
	str[0] = '\0';

	while(i < len){
		if(format[i] == '%'){
			int in_num = 0;
			char in_char[50] = "";
			switch(format[i+1]){
				case 'd'://%d 
					in_num = va_arg(var_arg,int);
					itoa(in_char,in_num);
					strcat(str, in_char);
					in_size += strlen(in_char);
					break;
				case 's'://%s
				 	strcpy(in_char,va_arg(var_arg, char*));
					strcat(str,in_char);
					in_size += strlen(in_char);
					break;
				default: printf("Not implememted argument type!\n");
			}
			i += 2;
		}
		else {
			char temp[2];
			temp[0] = format[i];
			temp[1] = '\0';
			strcat(str, temp);
			in_size ++;
			i ++;
		}
	}
		va_end(var_arg);
		return in_size;
}


int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
