#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/pcre/php_pcre.h"
#include "zend_exceptions.h"

#include "phiz_cregx_arginfo.h"
#include "phiz_cregx.h"
#include "php_phiz.h"

#include "Zend/zend_interfaces.h"
#include "Zend/zend_smart_str.h"

#include "src/tom_p.h"
#include "src/daytime.h"
#include "phiz_ctoml_arginfo.h"
#include "phiz_ctoml.h"

zend_object_handlers 	phiz_handler_Ctoml;
PHPAPI zend_class_entry *phiz_ce_Ctoml;


static void phiz_ctoml_copy_ctor( pz_tomp to, pz_tomp from)
{

}
static zend_object *phiz_ctoml_new_ex(zend_class_entry *class_type,
									zend_object *orig, bool clone_orig)
{
	pz_tomp 		  	   intern;
	zend_class_entry      *parent = class_type;
	bool                   inherited = false;

	intern = zend_object_alloc(sizeof(phiz_ctoml), class_type);

	ts_init_ts(&intern->ts);

	zend_object_std_init(&intern->std, class_type);

	object_properties_init(&intern->std, class_type);

	if (orig && clone_orig) {
		pz_tomp other = phiz_ctoml_from_obj(orig);
		phiz_ctoml_copy_ctor(intern, other);
	}
	while(parent) {
		if (parent == phiz_ce_Ctoml) {
			intern->std.handlers = &phiz_handler_Ctoml;
			break;
		}
		parent = parent->parent;
		inherited = true;
	}

	ZEND_ASSERT(parent);

	return &intern->std;
	
}


static zend_object *phiz_ctoml_new(zend_class_entry *class_type)
{
	return phiz_ctoml_new_ex(class_type, NULL, 0);
}

static void phiz_ctoml_free_storage(zend_object *object)
{
	pz_tomp intern = phiz_ctoml_from_obj(object);
	
	ts_destroy_ts(&intern->ts);
	zend_object_std_dtor(&intern->std);
}

static zend_object *phiz_ctoml_clone(zend_object *old_object)
{
	zend_object *new_object = phiz_ctoml_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

PHP_MINIT_FUNCTION(phiz_ctoml)
{
	phiz_register_std_class(&phiz_ce_Ctoml, "Ctoml", class_Ctoml_methods);

	phiz_ce_Ctoml->create_object = phiz_ctoml_new;


	memcpy(&phiz_handler_Ctoml, &std_object_handlers, sizeof(zend_object_handlers));

	phiz_handler_Ctoml.offset = XtOffsetOf(phiz_ctoml, std);
	phiz_handler_Ctoml.clone_obj = phiz_ctoml_clone;
	phiz_handler_Ctoml.dtor_obj  = zend_objects_destroy_object;
	phiz_handler_Ctoml.free_obj  = phiz_ctoml_free_storage;
}

PHP_METHOD(Ctoml, matchInt)
{

	zval *object = ZEND_THIS;
	zend_string*   subject;

	pz_tomp	   intern = Z_PHIZ_TOMP_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();

	bool isPartial = false;
	toml_stream* oo = &intern->ts;
	
	//printf("subject refct = %d interned=%d\n", subject->gc, ZSTR_IS_INTERNED(subject));
	//ZVAL_LONG(return_value,101);
	
	ts_parse_init(oo,NULL);
	ts_assign_value(oo, subject);

	if (!ts_match_integer(oo, return_value, &isPartial)) {
		ts_nomatch_error(oo);
	}
	else if (isPartial) {
		ts_partial_error(oo);
	}

	ts_handle_error(oo);
	ts_parse_end(oo);
}

PHP_METHOD(Ctoml, parse)
{
	zval *object = ZEND_THIS;
	zend_string*   subject;

	pz_tomp	   intern = Z_PHIZ_TOMP_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();


	HashTable* root = ts_parse_string(&intern->ts, subject);

    ZVAL_ARR(return_value, root);
	
	ts_handle_error(&intern->ts);

}


PHP_METHOD(Ctoml, matchBase)
{
	zval *object = ZEND_THIS;
	zend_string*   subject;

	pz_tomp	   intern = Z_PHIZ_TOMP_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();
	bool isPartial = false;
	toml_stream* oo = &intern->ts;
	ts_assign_value(oo, subject);
	//printf("subject refct = %d interned=%d\n", subject->gc, ZSTR_IS_INTERNED(subject));
	//ZVAL_LONG(return_value,101);
	ts_clear_error(oo);
	if (!ts_match_base(oo, return_value, &isPartial)) {
		ts_nomatch_error(oo);
	}
	else if (isPartial) {
		ts_partial_error(oo);
	}
	ts_handle_error(oo);

}

PHP_METHOD(Ctoml, matchBool)
{
	zval *object = ZEND_THIS;
	zend_string*   subject;

	pz_tomp	   intern = Z_PHIZ_TOMP_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();
	bool isPartial = false;
	toml_stream* oo = &intern->ts;
	ts_assign_value(oo, subject);
	//printf("subject refct = %d interned=%d\n", subject->gc, ZSTR_IS_INTERNED(subject));
	//ZVAL_LONG(return_value,101);
	ts_clear_error(oo);
	if (!ts_match_bool(oo, return_value, &isPartial)) {
		ts_nomatch_error(oo);
	}
	else if (isPartial) {
		ts_partial_error(oo);
	}
	ts_handle_error(oo);
}


PHP_METHOD(Ctoml, matchTime)
{
	zval *object = ZEND_THIS;
	zend_string*   subject;

	pz_tomp	   intern = Z_PHIZ_TOMP_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();
	bool isPartial = false;
	toml_stream* oo = &intern->ts;
	ts_assign_value(oo, subject);
	//printf("subject refct = %d interned=%d\n", subject->gc, ZSTR_IS_INTERNED(subject));
	//ZVAL_LONG(return_value,101);
	ts_clear_error(oo);
	if (!ts_match_daytime(oo, return_value, &isPartial)) {
		ts_nomatch_error(oo);
	}
	else if (isPartial) {
		ts_partial_error(oo);
	}
	ts_handle_error(oo);
}

PHP_METHOD(Ctoml, matchDateTime)
{
	zval *object = ZEND_THIS;
	zend_string*   subject;

	pz_tomp	   intern = Z_PHIZ_TOMP_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();
	bool isPartial = false;
	toml_stream* oo = &intern->ts;
	ts_assign_value(oo, subject);
	//printf("subject refct = %d interned=%d\n", subject->gc, ZSTR_IS_INTERNED(subject));
	//ZVAL_LONG(return_value,101);
	ts_clear_error(oo);
	if (!ts_match_datetime(oo, return_value, &isPartial)) {
		ts_nomatch_error(oo);
	}
	else if (isPartial) {
		ts_partial_error(oo);
	}
	ts_handle_error(oo);
}
PHP_METHOD(Ctoml, matchFloatExp)
{
	zval *object = ZEND_THIS;
	zend_string*   subject;

	pz_tomp	   intern = Z_PHIZ_TOMP_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();
	bool isPartial = false;
	toml_stream* oo = &intern->ts;
	ts_assign_value(oo, subject);
	//printf("subject refct = %d interned=%d\n", subject->gc, ZSTR_IS_INTERNED(subject));
	//ZVAL_LONG(return_value,101);
	ts_clear_error(oo);
	if (!ts_match_floatexp(oo, return_value, &isPartial)) {
		ts_nomatch_error(oo);
	}
	else if (isPartial) {
		ts_partial_error(oo);
	}
	ts_handle_error(oo);
}


PHP_METHOD(Ctoml, matchFloatDot)
{
	zval *object = ZEND_THIS;
	zend_string*   subject;

	pz_tomp	   intern = Z_PHIZ_TOMP_P(object);

	ZEND_PARSE_PARAMETERS_START(1, 1)
			Z_PARAM_STR(subject)
	ZEND_PARSE_PARAMETERS_END();
	bool isPartial = false;
	toml_stream* oo = &intern->ts;
	ts_assign_value(oo, subject);
	//printf("subject refct = %d interned=%d\n", subject->gc, ZSTR_IS_INTERNED(subject));
	//ZVAL_LONG(return_value,101);
	ts_clear_error(oo);
	if (!ts_match_floatdot(oo, return_value, &isPartial)) {
		ts_nomatch_error(oo);
	}
	else if (isPartial) {
		ts_partial_error(oo);
	}
	ts_handle_error(oo);

}
