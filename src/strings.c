#include <stdlib.h>
#include <stdio.h>
#include "strings.h"

int str_eql(const char* a, const char* b) {
	if (!a || !b) { return 0; }

	for(int len = 0; a[len] == b[len]; len++) {
		if (a[len] == 0 && b[len] == 0) {
			return 1;
		} else if (a[len] == 0 || b[len] == 0) {
			return 0;
		}
	}

	return 0;
}

int str_len(const char* s) {
	if (!s) { return 0; }

	int len = 0;
	while (s[++len] != 0) {
	}

	return len;
}

char * str_cat(const char* a, const char* b) {
	int la = a ? str_len(a) : 0;
	int lb = b ? str_len(b) : 0;
	printf("la: %d, lb: %d\n", la, lb);
	char* m = (char*)malloc(la + lb + 1);
	if (!m) {
		return NULL;
	}

	printf("add: %p, m: %s\n", (void*)m, m);
	str_copy(a, m, la);
	printf("add: %p, m: %s\n", (void*)(m + la + 1), m);
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

static int increase_array_size(char*** array, int current_size) {
	char** narray = (char**)malloc(sizeof(void*) * (current_size + 32));
	for(int i = 0; i < current_size; i++) {
		narray[i] = array[0][i]; 
	}

	free(*array);
	*array = narray;
	return current_size + 32;
}

struct str_array str_split(const char* src, const char* sep) {
	int sep_length = str_len(sep);
	int sep_count = 0;
	int max_count = 32;

	char* slice = (char*)malloc(sizeof(char) * (sep_length + 1));
	char** array = (char**)malloc(sizeof(void*) * 32);

	const char* current = src;
	const char* start = src;
	while (*(++current)) {
		str_copy(current, slice, sep_length);
		if (str_eql(sep, slice)) {
			if (sep_count + 1 >= max_count) {
				max_count = increase_array_size(&array, max_count);
			}

			unsigned long size = (unsigned long)current - (unsigned long)start;
			if (size > 0) {
				array[sep_count] = (char*)malloc(sizeof(char) * (size + 1));
				str_copy(start, array[sep_count], size);
				sep_count++;
			}

			start = current + 1;
		}
	}

	free(slice);
	struct str_array a = {.array = array, .length = sep_count};
	return a;
}

