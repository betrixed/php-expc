#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"

#ifndef PHIZ_DAYTIME_H
#include "daytime.h"
#endif



char* daytime_set(int hour, int min, double sec, double* ret) {
	if (hour < 0 || min < 0 || sec < 0.0) {
		return "daytime values cannot be negative";
	}
	if (hour >= 24) {
		return "hours must be less than 24";
	}
	if (min >= 60) {
		return "minutes must be less than 60";
	}
	if (sec >= 60.0) {
		return "seconds must be less than 60";	
	}
	*ret = (hour*60.0*60.0 + min*60.0 + sec)/(24.0*60*60);
	return NULL;
}

char* sto_daytime(char* s, long slen, double* ret) 
{
	long hours = 0;
	long mins = 0;
	double secs = 0.0;
	char* eptr;
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
	return daytime_set(hours,mins,secs,ret);
}


void daytime_split(double val, int* h24, int* min, double* sec) {
	double frs = val*24.0*60*60;
	*h24 = floor(frs/(60.0*60));
	*min = floor(frs/60.0) - (*h24)*60;
	*sec = frs - (*h24)*60.0*60 - (*min)*60;
}

zend_string*
daytime_format(double val, int flags){
	int h24 = 0;
	int min = 0;
	double sec = 0.0;
	daytime_split(val, &h24, &min, &sec);
	switch(flags) {
		case DAYT_SEC_INT:
			return strpprintf(0,"%02d:%02d:%02.0f",h24,min,sec);
			break;
		case DAYT_SEC_DEC:
			return strpprintf(0,"%02d:%02d:%08.6f",h24,min,sec);
			break;
		case DAYT_SEC_NONE:
			return strpprintf(0,"%02d:%02d",h24,min);
			break;
		case DAYT_SEC_AUTO:
		default:
			if (sec != 0.0) {
                if (sec - floor(sec) > 0.0) {
                     return strpprintf(0,"%02d:%02d:%8.6f",h24,min,sec);
                }
                else {
                    return strpprintf(0,"%02d:%02d:%02.0f",h24,min,sec);
                }
            }
            return strpprintf(0,"%02d:%02d", h24,min);
            break;
	}
}

