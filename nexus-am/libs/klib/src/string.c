#include <stddef.h>
#ifndef __ISA_NATIVE__

size_t strlen(const char *s) {
	size_t i = 0;
	while(*(s + i) != '\0'){
		i ++;
	}
  return i;
}

char* strcpy(char* dst,const char* src) {
	int i = 0;
	while(*(src + i) != '\0'){
		*(dst + i) = *(src + i);
		i ++;
	}
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
	size_t i;
	for(i = 0; i < n && src[i] != '\0'; ++i){
		dst[i] = src[i];
	}
	for( ; i < n; ++i){
		dst[i] = '\0';
	}
	return dst;
}

char* strcat(char* dst, const char* src) {
	size_t dst_len = strlen(dst);
	size_t i = 0;
	for(i = 0; src[i] != '\0'; ++i){
		dst[dst_len + i] = src[i];
	}
	dst[dst_len + i] = '\0';
	return dst;
}

int strcmp(const char* s1, const char* s2) {
	int len1 = strlen(s1);
	int len2 = strlen(s2);
	int len = 0;
	if(len1 < len2) len = len1;
	else len = len2;
	for(int i = 0; i <= len; ++i){
		if(s1[i] < s2[i]) return -1;
		else if(s1[i] > s2[i]) return 1;
	}
	return 0;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	for(size_t i = 0; i < n; ++i){
		if(s1[i] < s2[i]) return -1;
		else if(s1[i] > s2[i]) return 1;
	}
  return 0;
}

void* memset(void* s,int c,size_t n) {
	if(s == NULL || n < 0) return NULL;
	char* temp = (char*) s;
	for(int i = 0; i < n; ++i){
		temp[i] = (char) c;
	}
	return s;
}

void* memcpy(void* dest, const void* src, size_t n) {
	if(dest == NULL || n < 0) return NULL;
	char* chardest = (char*) dest;
	char* charsrc = (char*) src;
	for(int i = 0; i < n; ++i){
		chardest[i] = charsrc[i];
	}
	return dest;
}

int memcmp(const void* s1, const void* s2, size_t n){
	char* ch1 = (char*) s1;
	char* ch2 = (char*) s2;
	for(int i = 0; i < n; ++i){
		if(ch1[i] < ch2[i]) return -1;
		else if(ch1[i] > ch2[i]) return 1;
	}
	return 0;
}

#endif
