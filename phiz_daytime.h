#ifndef PHIZ_DAYTIME_H
#define PHIZ_DAYTIME_H

typedef struct _day_time {
	double	tval;
}
day_time;

typedef struct _phiz_daytime* pz_daytime;

typedef struct _phiz_daytime {
	day_time			 daytime;
	zend_object          std;
} phiz_daytime;


extern PHPAPI zend_class_entry *phiz_ce_DayTime;

static inline  pz_daytime phiz_daytime_from_obj(zend_object *obj)
{
	return (pz_daytime)((char*)(obj) - XtOffsetOf(phiz_daytime, std));
}

static inline  pz_daytime Z_PHIZ_DAYTIME_P(zval *zv)
{
	return (pz_daytime)((char*) Z_OBJ_P((zv)) - XtOffsetOf(phiz_daytime, std));
}

PHP_MINIT_FUNCTION(phiz_daytime);


#endif