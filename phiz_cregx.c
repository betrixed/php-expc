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

// ** how otherwise can the refcount be tweaked?
struct _pcre_cache_entry {
	pcre2_code *re;
	uint32_t preg_options;
	uint32_t capture_count;
	uint32_t name_count;
	uint32_t compile_options;
	uint32_t refcount;
};

zend_object_handlers 	phiz_handler_Cregx;
zend_object_handlers 	phiz_handler_Ctoml;


PHPAPI zend_class_entry *phiz_ce_Cregx;
PHPAPI zend_class_entry *phiz_ce_Ctoml;

static void phiz_cregx_copy_ctor( pz_cregx to, pz_cregx from)
{

}

static zend_object *phiz_cregx_new_ex(zend_class_entry *class_type,
									zend_object *orig, bool clone_orig)
{
	pz_cregx 		  	   intern;
	zend_class_entry      *parent = class_type;
	bool                   inherited = false;

	intern = zend_object_alloc(sizeof(phiz_cregx), class_type);

	ZVAL_NULL(&intern->expr);
	ZVAL_NULL(&intern->subpats);
	intern->flags = 0;
	intern->global = 0;


	zend_object_std_init(&intern->std, class_type);

	object_properties_init(&intern->std, class_type);

	if (orig && clone_orig) {
		pz_cregx other = phiz_cregx_from_obj(orig);
		phiz_cregx_copy_ctor(intern, other);
	}
	while(parent) {
		if (parent == phiz_ce_Cregx) {
			intern->std.handlers = &phiz_handler_Cregx;
			break;
		}
		parent = parent->parent;
		inherited = true;
	}

	ZEND_ASSERT(parent);

	return &intern->std;
	
}

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

	ts_init_ts(&intern->ts, NULL);

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


static zend_object *phiz_cregx_new(zend_class_entry *class_type)
{
	return phiz_cregx_new_ex(class_type, NULL, 0);
}

static zend_object *phiz_cregx_clone(zend_object *old_object)
{
	zend_object *new_object = phiz_cregx_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static void phiz_cregx_free_storage(zend_object *object)
{
	pz_cregx intern = phiz_cregx_from_obj(object);
	
	Z_TRY_DELREF(intern->expr);
	Z_TRY_DELREF(intern->subpats);

	zend_object_std_dtor(&intern->std);
}

PHP_METHOD(Cregx, __construct)
{
	zval *object = ZEND_THIS;
	zend_string*   expr;
	zend_long     flags = 0;
	zend_long     global = 0;
	pz_cregx	   intern;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(expr)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(global)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CREGX_P(object);
	ZVAL_STR(&intern->expr, expr);
	intern->flags = flags;
	intern->global = global;
}


PHP_METHOD(Cregx, setExpr)
{
	zval *object = ZEND_THIS;
	zend_string*   expr;
	zend_long     flags = 0;
	zend_long     global = 0;
	pz_cregx	   intern;

	ZEND_PARSE_PARAMETERS_START(1, 3)
		Z_PARAM_STR(expr)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
		Z_PARAM_LONG(global)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CREGX_P(object);
	Z_TRY_DELREF(intern->expr);
	ZVAL_STR(&intern->expr, expr);
	intern->flags = flags;
	intern->global = global;
}


PHP_METHOD(Cregx, __toString) {
		zval *object = ZEND_THIS;
		pz_cregx	   intern;

		intern = Z_PHIZ_CREGX_P(object);

		ZEND_PARSE_PARAMETERS_NONE();

		ZVAL_COPY(return_value, &intern->expr);
}

PHP_METHOD(Cregx, match) {
		zval *object = ZEND_THIS;
		pz_cregx	   intern;
		zend_string	   *subject;
		zend_long      offset = 0;      
		pcre_cache_entry* pce;

		intern = Z_PHIZ_CREGX_P(object);

		ZEND_PARSE_PARAMETERS_START(2, 2)
			Z_PARAM_STR(subject)
			Z_PARAM_LONG(offset)
		ZEND_PARSE_PARAMETERS_END();


	if (Z_TYPE(intern->subpats) == IS_ARRAY) {
		zval_ptr_dtor(&intern->subpats);
		ZVAL_NULL(&intern->subpats);
	}
	

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(Z_STR(intern->expr))) == NULL) {
		RETURN_LONG(0);
	}

	pce->refcount++;
	php_pcre_match_impl(pce, subject, return_value, &intern->subpats,
		intern->global, ZEND_NUM_ARGS() >= 4, intern->flags, offset);
	pce->refcount--;
	if (Z_TYPE(intern->subpats) == IS_ARRAY) {
		Z_ADDREF_P(&intern->subpats);
	}
}

PHP_METHOD(Cregx, captures)
{
	zval *object = ZEND_THIS;
	pz_cregx	   intern;

	intern = Z_PHIZ_CREGX_P(object);

	ZEND_PARSE_PARAMETERS_NONE();

	ZVAL_COPY_VALUE(return_value, &intern->subpats);

}

PHP_METHOD(Cregx, clear)
{
	zval *object = ZEND_THIS;
	pz_cregx	   intern;
	
	intern = Z_PHIZ_CREGX_P(object);

	ZEND_PARSE_PARAMETERS_NONE();

	if (Z_TYPE(intern->subpats) == IS_ARRAY) {
		zval_ptr_dtor(&intern->subpats);
		ZVAL_NULL(&intern->subpats);
	}

	ZVAL_NULL(return_value);

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
	ts_assign_value(oo, subject);
	//printf("subject refct = %d interned=%d\n", subject->gc, ZSTR_IS_INTERNED(subject));
	//ZVAL_LONG(return_value,101);
	ts_clear_error(oo);
	if (!ts_match_integer(oo, return_value, &isPartial)) {
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


PHP_MINIT_FUNCTION(phiz_cregx)
{
	phiz_register_std_class(&phiz_ce_Cregx, "Cregx", class_Cregx_methods);

	phiz_ce_Cregx->create_object = phiz_cregx_new;

	memcpy(&phiz_handler_Cregx, &std_object_handlers, sizeof(zend_object_handlers));

	phiz_handler_Cregx.offset = XtOffsetOf(phiz_cregx, std);
	phiz_handler_Cregx.clone_obj = phiz_cregx_clone;
	phiz_handler_Cregx.dtor_obj  = zend_objects_destroy_object;
	phiz_handler_Cregx.free_obj  = phiz_cregx_free_storage;
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