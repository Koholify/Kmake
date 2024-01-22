#include <stdlib.h>
#include <stdio.h>
#include "strings.h"

int str_eql(const char* a, const char* b) {
	if (!a || !b) { return 0; }

	for(int len = 0; a[len] == b[len] && a[len] != 0; len++) {
		if (a[len] == 0 || b[len] == 0) {
			return 0;
		}
	}

	return 1;
}

int str_len(const char* s) {
	if (!s) { return 0; }

	int len = 0;
	while (s[len++] != 0) {
	}

	return len;
}

char * str_cat(const char* a, const char* b) {
	int la = a ? str_len(a) : 0;
	int lb = b ? str_len(b) : 0;
	printf("la: %d, lb: %d\n", la, lb);
	char* m = (char*)malloc(la + lb);
	if (!m) {
		return NULL;
	}

	printf("add: %p, m: %s\n", (void*)m, m);
	str_copy(a, m, la);
	printf("add: %p, m: %s\n", (void*)(m + la), m);
	str_copy(b, m + la, lb);
	printf("m: %s\n", m);

	return m;
}

char * str_copy(const char* src, char* dest, int len) {
	for (int i = 0; i < len; i++) {
		dest[i] = src[i];
	}
	return dest;
}
struct str_array str_split(const char* src, const char* sep) {
	struct str_array a = { 0, 0 };
	return a;
}

