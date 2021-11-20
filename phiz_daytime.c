#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/pcre/php_pcre.h"
#include "zend_exceptions.h"

#include "Zend/zend_interfaces.h"
#include "Zend/zend_smart_str.h"
#include "Zend/zend_types.h"
#include "daytime_arginfo.h"
#include "src/daytime.h"
#include "phiz_carray.h"
#include "phiz_daytime.h"
#include "php_phiz.h"

zend_object_handlers 	phiz_handler_DayTime;

zend_class_entry *phiz_ce_DayTime;

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
	zval*   hours;
	zval*   mins;
	zval*   secs;

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
	double ret;
	

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(hours)
		Z_PARAM_LONG(mins)
		Z_PARAM_DOUBLE(secs)
	ZEND_PARSE_PARAMETERS_END();

	char* error = daytime_set(hours,mins,secs, &ret);	

	if (error) {
		zend_throw_exception(phiz_ce_RuntimeException,error,1);
	}
}

ZEND_FUNCTION(daytime_str) {
	char* src;
	long  slen;
	double ret;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(src, slen)
	ZEND_PARSE_PARAMETERS_END();

	char* error = sto_daytime(src, slen, &ret);
	ZVAL_DOUBLE(return_value, ret);

	if (error) {
		zend_throw_exception(phiz_ce_RuntimeException,error,1);
	}
}

PHP_METHOD(DayTime, __construct)
{
	zval *object = ZEND_THIS;
	char* src;
	long  slen;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(src, slen)
	ZEND_PARSE_PARAMETERS_END();
	pz_daytime pz = Z_PHIZ_DAYTIME_P(object);

	char* error = sto_daytime(src, slen, &pz->daytime.tval);
	if (error) {
		zend_throw_exception(phiz_ce_RuntimeException,error,1);
	}	

}

PHP_METHOD(DayTime, set)
{
	zval *object = ZEND_THIS;
	long hours;
	long mins;
	double secs;	

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_LONG(hours)
		Z_PARAM_LONG(mins)
		Z_PARAM_DOUBLE(secs)
	ZEND_PARSE_PARAMETERS_END();
	pz_daytime pz = Z_PHIZ_DAYTIME_P(object);

	char* error = daytime_set(hours,mins,secs, &pz->daytime.tval);	
	
	if (error) {
		zend_throw_exception(phiz_ce_RuntimeException,error,1);
	}
}


PHP_METHOD(DayTime, split)
{
	zval *object = ZEND_THIS;
	zval*   hours;
	zval*   mins;
	zval*   secs;

	ZEND_PARSE_PARAMETERS_START(3, 3)
	Z_PARAM_ZVAL(hours)
	Z_PARAM_ZVAL(mins)
	Z_PARAM_ZVAL(secs)
	ZEND_PARSE_PARAMETERS_END();
	int h24, m60;
	double s60;
	pz_daytime pz = Z_PHIZ_DAYTIME_P(object);
	daytime_split(pz->daytime.tval, &h24, &m60, &s60);
	ZVAL_LONG(hours, h24);
	ZVAL_LONG(mins, m60);
	ZVAL_DOUBLE(secs, s60);
}

PHP_METHOD(DayTime, format)
{
	zval *object = ZEND_THIS;
	long 	flags = 0;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();
	pz_daytime pz = Z_PHIZ_DAYTIME_P(object);

	result = daytime_format(pz->daytime.tval,flags);
	ZVAL_STR(return_value, result);

}

PHP_METHOD(DayTime, __toString)
{
	zval *object = ZEND_THIS;
	zend_string *result;

	ZEND_PARSE_PARAMETERS_NONE();
	
	pz_daytime pz = Z_PHIZ_DAYTIME_P(object);
	result = daytime_format(pz->daytime.tval,DAYT_SEC_AUTO);
	ZVAL_STR(return_value, result);

}


static void phiz_daytime_copy_ctor( pz_daytime to, pz_daytime from)
{

}

static zend_object *phiz_daytime_new_ex(zend_class_entry *class_type,
									zend_object *orig, bool clone_orig)
{
	pz_daytime 		  	   intern;
	zend_class_entry      *parent = class_type;
	bool                   inherited = false;

	intern = zend_object_alloc(sizeof(phiz_daytime), class_type);
	intern->daytime.tval = 0.0;


	zend_object_std_init(&intern->std, class_type);

	object_properties_init(&intern->std, class_type);

	if (orig && clone_orig) {
		pz_daytime other = phiz_daytime_from_obj(orig);
		phiz_daytime_copy_ctor(intern, other);
	}
	while(parent) {
		if (parent == phiz_ce_DayTime) {
			intern->std.handlers = &phiz_handler_DayTime;
			break;
		}
		parent = parent->parent;
		inherited = true;
	}

	ZEND_ASSERT(parent);

	return &intern->std;
	
}



static zend_object *phiz_daytime_new(zend_class_entry *class_type)
{
	return phiz_daytime_new_ex(class_type, NULL, 0);
}

static zend_object *phiz_daytime_clone(zend_object *old_object)
{
	zend_object *new_object = phiz_daytime_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static void phiz_daytime_free_storage(zend_object *object)
{
	pz_daytime intern = phiz_daytime_from_obj(object);

	zend_object_std_dtor(&intern->std);
}

PHP_MINIT_FUNCTION(phiz_daytime)
{
	phiz_register_std_class(&phiz_ce_DayTime, "DayTime", class_DayTime_methods);

	phiz_ce_DayTime->create_object = phiz_daytime_new;

	memcpy(&phiz_handler_DayTime, &std_object_handlers, sizeof(zend_object_handlers));

	phiz_handler_DayTime.offset = XtOffsetOf(phiz_daytime, std);
	phiz_handler_DayTime.clone_obj = phiz_daytime_clone;
	phiz_handler_DayTime.dtor_obj  = zend_objects_destroy_object;
	phiz_handler_DayTime.free_obj  = phiz_daytime_free_storage;
}