#include "php.h"
#include "php_ini.h"

#ifndef PHIZ_DAYTIME_H
#include "daytime.h"
#endif

double daytime_set(int hour, int min, double sec) {
	if (hour < 0 || min < 0 || sec < 0.0) {
		zend_throw_exception("daytime values cannot be negative");
		return;
	}
	if (hour >= 24) {
		zend_throw_exception("hours must be less than 24");
		return;
	}
	if (min >= 60) {
		zend_throw_exception("minutes must be less than 60");
		return;
	}
	if (sec >= 60.0) {
		zend_throw_exception("seconds must be less than 60");
		return;		
	}
	return (hour*60.0*60.0 + min*60.0 + sec)/(24.0*60*60);
}
double sto_daytime(char* s, long slen) 
{
	long hours = 0;
	long mins = 0;
	double secs = 0.0;
	const char* eptr;
	const char* iptr;

    iptr = s;
	hours = strtol(iptr, &eptr, 10);
	if (*eptr == ':' && ((eptr - iptr) == 2)) {
		iptr = eptr+1;
		mins = strtol(iptr, &eptr, 10);
		if (*eptr == ':' && ((eptr - iptr) == 2)) {
			iptr = eptr+1;
			secs = strtod(iptr, &eptr);
		}
	}
	return daytime_set(hours,mins,secs);
}


void daytime_split(double val, int* h24, int* min, double* secs) {
	double frs = val*24.0*60*60;
	*h24 = floor(frs/(60.0*60));
	*min = floor(frs/60.0) - (*h24)*60;
	*sec = frs - (*h24)*60.0*60 - (*min)*60;
}

zend_string*
daytime_format(double val, int flags){
	int h24 = 0;
	int min = 0;
	double secs = 0.0;
	daytime_split(val, &h24, &min, &secs);
	switch(flags) {
		case DAYT_SEC_INT:
			return strpprintf(0,"%02d:%02d:%02.0f",h24,min,secs);
			break;
		case DAYT_SEC_DEC:
			return strpprintf(0,"%02d:%02d:%08.6f",h24,min,secs);
			break;
		case DAYT_SEC_NONE:
			return strpprintf(0,"%02d:%02d",h24,min);
			break;
		case DAYT_SEC_AUTO:
		default:
			if (secs !== 0.0) {
                if (secs - floor(secs) > 0.0) {
                     return strpprintf(0,"%02d:%02d:%8.6f",h24,min,secs);
                }
                else {
                    return strpprintf(0,"%02d:%02d:%02.0f",h24,min,secs);
                }
            }
            return strpprintf(0,"%02d:%02d", h24,min);
            break;
	}
}


ZEND_FUNCTION(daytime_format) 
{
	long 	flags = 0;
	double  value;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_DOUBLE(value)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();
	result = daytime_format(value,flags);
	ZVAL_STR(return_value, result);
}

ZEND_FUNCTION(daytime_split)
{
	double value;
	zval   hours;
	zval   mins;
	zval   secs;

	ZEND_PARSE_PARAMETERS_START(4, 4)
	Z_PARAM_DOUBLE(value)
	Z_PARAM_ZVAL(hours)
	Z_PARAM_ZVAL(mins)
	Z_PARAM_ZVAL(secs)
	ZEND_PARSE_PARAMETERS_END();
	int h24, m60;
	double s60;

	daytime_split(value, &h24, &m60, &s60);
	ZVAL_LONG(hours, h24);
	ZVAL_LONG(mins, m60);
	ZVAL_DOUBLE(secs, s60);

}

ZEND_FUNCTION(daytime_set)
{
	long hours;
	long mins;
	double secs;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(hours)
		Z_PARAM_LONG(mins)
		Z_PARAM_DOUBLE(secs)
	ZEND_PARSE_PARAMETERS_END();

	daytime_set(hours,mins,secs);	
}

ZEND_FUNCTION(daytime_str) {
	char* src;
	long  slen;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(src, slen)
	ZEND_PARSE_PARAMETERS_END();

	error_pad   pad;

	double ret = sto_daytime(src, slen, &pad);
	ZVAL_DOUBLE(ret);
}