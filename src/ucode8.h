#ifndef _H_UCODE8
#define _H_UCODE8

#include <uchar.h>

//! return number of character units consumed, and unicode code value
unsigned int ucode8Fore(
	char const* cpmt, // buffer pointer
	unsigned int slen, // size
	char32_t* d       // 32-bit unicode value
	);
extern const char32_t INVALID_CHAR;

unsigned int utf32_str8(char32_t d, char (*result) [8]);

#endif
