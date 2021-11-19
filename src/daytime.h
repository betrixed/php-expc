#ifndef PHIZ_DAYTIME_H
#define PHIZ_DAYTIME_H

typedef struct _day_time {
	double	tval;
}
day_time;

enum {
	DAYT_SEC_AUTO = 0;
	DAYT_SEC_INT = 1;
	DAYT_SEC_DEC = 2;
	DAYT_SEC_NONE = 4;
}
typedef struct _day_time* pz_daytm;

typedef struct _phiz_ctoml {
	day_time			 daytime;
	zend_object          std;
} phiz_ctoml;


extern PHPAPI zend_class_entry *phiz_ce_Tomp;

PHP_MINIT_FUNCTION(phiz_ctoml);

static inline  pz_daytm phiz_daytm_from_obj(zend_object *obj)
{
	return (pz_daytm)((char*)(obj) - XtOffsetOf(day_time, std));
}

static inline  pz_daytm Z_PHIZ_DAYTM_P(zval *zv)
{
	return (pz_daytm)((char*) Z_OBJ_P((zv)) - XtOffsetOf(day_time, std));
}

double daytime_set(int hours, int mins, double secs);
double sto_daytime(char* s, long slen);
void daytime_split(double val, int* h24, int* min, double* secs);
zend_string* daytime_format(double val, int flags)