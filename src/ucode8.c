
/** 
    For PHP
    Have to detect valid character, construct the unicode character number
    , and  buffer
    into character buffer during the process, with a terminal 0.
    Length of buffer also calculated from change in index
*/

#include "ucode8.h"

const char32_t INVALID_CHAR = (char32_t) -1;
/**
 * char const*  cpt      pointer to first character to consume
 * unsigned int  slen    offset of EOS from cpt
 * char32_t*     uc      next unicode character number returned, or INVALID_CHAR
 * @return unsigned int  number of bytes consumed
 */

unsigned int
    ucode8Fore(
	char const* cpt, 
	unsigned int slen, 
	char32_t* uc)
{            
    if (!cpt || slen == 0) {
        *uc = INVALID_CHAR;
        return 0;
    }
    const unsigned char* pa = (unsigned char const*)(cpt);

    unsigned int k = 0;
    if (k >= slen)
        return 0;
    unsigned char temp = pa[k++];
    char32_t test = temp;


    if (test < 0x80)
    {
        *uc = test; //1
        return k;
    }
            
    if (test < 0xC2)
        return k;
                
    if (test < 0xE0)
    {
        if (k >= slen) {
            *uc = INVALID_CHAR;
            return k;

        }
        temp = pa[k++];
        *uc = ((test & 0x1F) << 6) + (temp & 0x3F);//2
        return k;
    }
    else if (test < 0xF0)
    {
        if (k >= slen) {
            *uc = INVALID_CHAR;
            return k;
        }
        temp = pa[k++];
        test = ((test & 0x0F) << 6) + (temp & 0x3F);//2
        if (k >= slen)
        {
            *uc = INVALID_CHAR;
            return k;
        }

        temp = pa[k++];
        *uc = (test << 6) + (temp & 0x3F);//3
        return k;
    }
    else if (test < 0xF5) // reach to 140000
    {
        if (k >= slen) {
            *uc = INVALID_CHAR;
            return k;
        }
        temp = pa[k++];

        test = ((test & 0x07) << 6) + (temp & 0x3F);//2
        if (k >= slen) {
            *uc = INVALID_CHAR;
            return k;
        }
        temp = pa[k++];
        test = (test << 6) + (temp & 0x3F);//3
        if (k >= slen) {
            *uc = INVALID_CHAR;
            return k;
        }
        temp = pa[k++];
        test = (test << 6) + (temp & 0x3F);//4
        if (test > 0x10FFFF) {
            *uc = INVALID_CHAR;
            return k;
        }
        *uc = test;
        return k;
    }
    else {
        *uc = INVALID_CHAR;
        return k;
    }
}

// return number of char put in result
unsigned int
utf32_str8(char32_t d, char (*result) [8])
{
    if (d < 0x80)
    {
        (*result)[0] = (char) d;
        return 1;
    }
    if (d < 0x800)
    {
        // encode in 11 bits, 2 bytes
        (*result)[1] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[0] = (char) (d | 0xC0);
        return 2;
    }
    else if (d < 0x10000)
    {
        // encode in 16 bits, 3 bytes
        (*result)[2] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[1] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[0] = (char) (d | 0xE0);
        return 3;
    }
    else if (d > 0x10FFFF)
    {
        // not in current unicode range?
        return 0;
    }
    else {
        // encode in 21 bits, 4 bytes
        (*result)[3] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[2] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[1] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[0] = (char) (d | 0xF0);
        return 4;
    }
}