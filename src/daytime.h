#ifndef SRC_DAYTIME_H
#define SRC_DAYTIME_H



enum {
	DAYT_SEC_AUTO = 0,
	DAYT_SEC_INT = 1,
	DAYT_SEC_DEC = 2,
	DAYT_SEC_NONE = 4
};


char* daytime_set(int hours, int mins, double secs, double* ret);
char* sto_daytime(char* s, long slen, double* ret);
void daytime_split(double val, int* h24, int* min, double* secs);
zend_string* daytime_format(double val, int flags);


#endif