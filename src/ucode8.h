#ifndef _H_UCODE8
#define _H_UCODE8

#include <uchar.h>

//! return number of character units consumed, and unicode code value
unsigned int ucode8Fore(
	char const* cpt, // buffer pointer
	unsigned int slen, // size
	char32_t* d       // 32-bit unicode value
	);
const char32_t INVALID_CHAR;

#endif