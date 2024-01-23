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
	dest[len] = 0;

	return dest;
}

char * str_acopy(const char* src) {
	int len = str_len(src);
	char* str = (char*)malloc(sizeof(char) * (len + 1));
	str_copy(src, str, len);
	return str;
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
	return str_splitN(src, sep, -1);
}

struct str_array str_splitN(const char* src, const char* sep, int n) {
	int sep_length = str_len(sep);
	int sep_count = 0;
	int max_count = 32;

	char* slice = (char*)malloc(sizeof(char) * (sep_length + 1));
	char** array = (char**)malloc(sizeof(void*) * 32);

	const char* current = src;
	const char* start = src;
	while (*(++current) && (n < 1 || sep_count < n)) {
		str_copy(current, slice, sep_length);
		if (str_eql(sep, slice)) {
			if (sep_count + 1 >= max_count) {
				max_count = increase_array_size(&array, max_count);
			}

			unsigned long size = (unsigned long long)current - (unsigned long long)start;
			if (size > 0) {
				array[sep_count] = (char*)malloc(sizeof(char) * (size + 1));
				str_copy(start, array[sep_count], size);
				sep_count++;
			}

			start = current + 1;
		}
	}

	if (sep_count > 0 && sep_count >= n) {
		while(*(++current)) {}
	}

	if (start != current) {
			unsigned long size = (unsigned long long)current - (unsigned long long)start;
			array[sep_count] = (char*)malloc(sizeof(char) * (size + 1));
			str_copy(start, array[sep_count], size);
			sep_count++;
	}

	free(slice);
	struct str_array a = {.array = array, .length = sep_count};
	return a;
}

struct str_stream* str_stream_init(void) {
	str_stream* stream = (str_stream*)malloc(sizeof(str_stream));
	stream->end = stream;
	stream->length = 0;
	return stream;
}

void str_stream_add(struct str_stream* stream, const char* str) {
	int len = str_len(str);
	if (len == 0) return;

	str_stream* next = (str_stream*)malloc(sizeof(str_stream));
	next->end = next;
	next->next = 0;
	next->str = 0;
	next->length = len;

	if (len > 0) {
		next->str = (char*)malloc(sizeof(char) * (len + 1));
		str_copy(str, next->str, len);
	}

	stream->end->next = next;
	stream->end = next;
	stream->length += len;
}

char* str_stream_merge(struct str_stream* stream) {
	unsigned long length = stream->length;
	str_stream* current = stream;
	char* result = (char*)malloc(sizeof(char) * (length + 1));
	int c = 0;
	current = stream->next;
	while (current) {
		if (current->length > 0) {
			str_copy(current->str, result + c, current->length);
			c += current->length;
		}

		current = current->next;
	}

	return result;
}

void str_stream_free(struct str_stream* stream) {
	while (stream) {
		free(stream->str);
		str_stream* next = stream->next;
		free(stream);
		stream = next;
	}
}

