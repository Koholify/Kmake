#ifndef _H_STRINGS
#define _H_STRINGS

struct str_array {
	char ** array;
	int length;
};

int str_eql(const char* a, const char* b);
int str_len(const char* s);

// allocate copy of string a+b on the heap
char * str_cat(const char* a, const char* b);
// copy data from dest starting at src for len characters
char * str_copy(const char* src, char* dest, int len);
//Not implemented
struct str_array str_split(const char* src, const char* sep);

#endif
