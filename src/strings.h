#ifndef _H_STRINGS
#define _H_STRINGS

struct str_stream {
	char * str;
	unsigned long length;
	struct str_stream* next;
	struct str_stream* end;
};

typedef struct str_stream str_stream;

struct str_array {
	char ** array;
	int length;
};

typedef struct str_array str_array;

int str_eql(const char* a, const char* b);
int str_len(const char* s);

// allocate copy of string a+b on the heap
char * str_cat(const char* a, const char* b);
// copy data from dest starting at src for len characters
char * str_copy(const char* src, char* dest, int len);
// copy data from src and allocate into new string
char * str_acopy(const char* src);
// Split a string into an array of strings.
struct str_array str_split(const char* src, const char* sep);
struct str_array str_splitN(const char* src, const char* sep, int n);

// Create root node of str_stream
struct str_stream* str_stream_init(void);
// Add a section to the stream
void str_stream_add(struct str_stream* stream, const char* str);
// Merge the stream into a single stream
char* str_stream_merge(struct str_stream* stream);
// Free allocated structure
void str_stream_free(struct str_stream* stream);

#endif
