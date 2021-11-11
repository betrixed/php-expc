#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "phiz_cregx_arginfo.h"
#include "phiz_cregx.h"
#include "php_phiz.h"

#include "Zend/zend_interfaces.h"
#include "Zend/zend_smart_str.h"


zend_object_handlers 	phiz_handler_Cregx;


PHPAPI zend_class_entry *phiz_ce_Cregx;

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

	ZVAL_NULL(&intern->options);

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
	Z_TRY_DELREF(intern->options);

	zend_object_std_dtor(&intern->std);
}

PHP_METHOD(Cregx, __construct)
{
	zval *object = ZEND_THIS;
	zend_string*   expr;
	zend_string*   options;

	pz_cregx	   intern;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(expr)
		Z_PARAM_STR(options)
	ZEND_PARSE_PARAMETERS_END();


	intern = Z_PHIZ_CREGX_P(object);
	ZVAL_STR(&intern->expr, expr);
	ZVAL_STR(&intern->options, options);

}

PHP_METHOD(Cregx, __toString) {
		zval *object = ZEND_THIS;
		pz_cregx	   intern;

		intern = Z_PHIZ_CREGX_P(object);

		ZEND_PARSE_PARAMETERS_NONE();

		ZVAL_COPY_VALUE(return_value, &intern->expr);
}

PHP_METHOD(Cregx, match) {
		zval *object = ZEND_THIS;
		pz_cregx	   intern;
		zend_string	   *subject;
		zend_long      offset;      

		intern = Z_PHIZ_CREGX_P(object);

		ZEND_PARSE_PARAMETERS_START(1, 2)
			Z_PARAM_STR(subject)
			Z_PARAM_LONG(offset)
		ZEND_PARSE_PARAMETERS_END();

		array_init(return_value);
}

PHP_MINIT_FUNCTION(phiz_cregx)
{
	phiz_register_std_class(&phiz_ce_Cregx, "Cregx", class_Cregx_methods);

	phiz_ce_Cregx->create_object = phiz_cregx_new;

	memcpy(&phiz_ce_Cregx, &std_object_handlers, sizeof(zend_object_handlers));

	phiz_handler_Cregx.offset = XtOffsetOf(phiz_cregx, std);
	phiz_handler_Cregx.clone_obj = phiz_cregx_clone;
	phiz_handler_Cregx.dtor_obj  = zend_objects_destroy_object;
	phiz_handler_Cregx.free_obj  = phiz_cregx_free_storage;
}