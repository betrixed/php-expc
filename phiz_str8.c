#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "php.h"
#include "php_phiz.h"
#include "phiz_str8.h"
#include "phiz_str8_arginfo.h"
#include "Zend/zend_interfaces.h"
#include "src/str8_obj.h"

#define phiz_ce_Aggregate     zend_ce_aggregate

typedef struct _phiz_str8_obj {
	zval 		  target;
	zend_object   std;
} phiz_str8_obj;

typedef struct _str8_quickit {
	zend_object_iterator intern;
	zval                 result; // holder zval for current value
	str8_obj             str8;
} str8_quickit;


static inline phiz_str8_obj* 
phiz_str8_from_obj(zend_object *obj) /* {{{ */ {
	return (phiz_str8_obj*)((char*)(obj) - XtOffsetOf(phiz_str8_obj, std));
}

static inline phiz_str8_obj* 
Z_PHIZ_STR8_P(zval *zv) 
{
	return (phiz_str8_obj*)((char*)(Z_OBJ_P((zv))) - XtOffsetOf(phiz_str8_obj, std));
} 

// structure for pointer to functions
zend_object_handlers 	phiz_handler_Str8;

PHPAPI zend_class_entry *phiz_ce_Str8;

static void str8_quickit_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
	zval_ptr_dtor( &((str8_quickit*)iter)->result );
}

// all the work is done in the move forware
static void str8_quickit_move_forward(zend_object_iterator *iter)
{
	str8_quickit*	   iterator = (str8_quickit*)iter;
	phiz_str8_obj*     this   = Z_PHIZ_STR8_P(&iter->data);

	phiz_str8_move((p_str8) &iterator->str8);

}

static void str8_quickit_rewind(zend_object_iterator *iter)
{
	str8_quickit*	   iterator = (str8_quickit*)iter;
    phiz_str8_rewind(&iterator->str8);
	phiz_str8_move(&iterator->str8);
}

static int str8_quickit_valid(zend_object_iterator *iter)
{
	str8_quickit*	   iterator = (str8_quickit*)iter;

	if (phiz_str8_valid(&iterator->str8)) {
		return SUCCESS;
	}
	return FAILURE;
}

static zval *str8_quickit_get_current_data(zend_object_iterator *iter)
{
	str8_quickit*	   iterator = (str8_quickit*)iter;

	//zindex = iterator->current;
	/* if (zindex < 0 || zindex >= object->cobj.size) {
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		return NULL;
	}
	*/
	zval* result = &iterator->result;
	ZVAL_LONG(result, iterator->str8.ucode);
	return result;
}

// return next rather than current. Offset to end of the utf8-code character.
// The first character offset is always 0. Store next as offset for next loop.
static void str8_quickit_get_current_key(zend_object_iterator *iter, zval *key)
{
	str8_quickit*	   iterator = (str8_quickit*)iter;
	ZVAL_LONG(key, 	   iterator->str8.next);
}



static zend_object_iterator_funcs str8_itftab = 
{
		str8_quickit_dtor,
		str8_quickit_valid,
		str8_quickit_get_current_data,
		str8_quickit_get_current_key,
		str8_quickit_move_forward,
		str8_quickit_rewind
		// , NULL, NULL
};

zend_object_iterator *str8_get_iterator(
	zend_class_entry *ce, zval *object, int by_ref)
{
	str8_quickit *iterator;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(str8_quickit));

	zend_iterator_init((zend_object_iterator*)iterator);
	ZVAL_UNDEF(&iterator->result);
	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	phiz_str8_obj*     this   = Z_PHIZ_STR8_P(object);

	iterator->intern.funcs = &str8_itftab;
	zend_string* s = Z_STR(this->target);
	phiz_str8_init(&iterator->str8, ZSTR_VAL(s), ZSTR_LEN(s));
	return (zend_object_iterator*)iterator;
}

/* {{{ Constructs a new Utf8 string iterator from a string */
PHP_METHOD(Str8, __construct)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;
	zend_string *target;
	zend_long ar_flags = 0;

	if (ZEND_NUM_ARGS() == 0) {
		return; /* nothing to do */
	}

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(target)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_STR8_P(object);
	ZVAL_STR_COPY(&intern->target, target);

}

/* Create a new iterator from instance. */
PHP_METHOD(Str8, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

/* {{{ Return the iterated string */
PHP_METHOD(Str8, __toString)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;

	intern = Z_PHIZ_STR8_P(object);
	
	ZVAL_COPY_VALUE(return_value, &intern->target);
}

// not implementing inherited (yet)
static zend_object *phiz_str8_new_ex(zend_class_entry *class_type,
									zend_object *orig, bool clone_orig)
{
	phiz_str8_obj   *intern;
	zend_class_entry      *parent = class_type;
	bool                   inherited = false;

	intern = zend_object_alloc(sizeof(phiz_str8_obj), class_type);
	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	while(parent) {
		if (parent == phiz_ce_Str8) {
			intern->std.handlers = &phiz_handler_Str8;
			break;
		}
		parent = parent->parent;
		inherited = true;
	}

	ZEND_ASSERT(parent);

	ZEND_ASSERT(!inherited);

	return &intern->std;
	
}

static zend_object *phiz_str8_new(zend_class_entry *class_type)
{
	return phiz_str8_new_ex(class_type, NULL, 0);
}

static zend_object *phiz_str8_clone(zend_object *old_object)
{
	zend_object *new_object = phiz_str8_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}

static void phiz_str8_free_storage(zend_object *object)
{
	phiz_str8_obj* intern = phiz_str8_from_obj(object);

	Z_TRY_DELREF(intern->target);
	zend_object_std_dtor(&intern->std);
}

PHP_MINIT_FUNCTION(phiz_str8)
{

	phiz_register_std_class(&phiz_ce_Str8, "Str8", 
		phiz_str8_new, class_Str8_methods);

	memcpy(&phiz_handler_Str8, &std_object_handlers, sizeof(zend_object_handlers));

	phiz_handler_Str8.offset = XtOffsetOf(phiz_str8_obj, std);
	phiz_handler_Str8.clone_obj = phiz_str8_clone;
	phiz_handler_Str8.dtor_obj  = zend_objects_destroy_object;
	phiz_handler_Str8.free_obj  = phiz_str8_free_storage;

	REGISTER_PHIZ_IMPLEMENTS(Str8, Aggregate);

	phiz_ce_Str8->get_iterator = str8_get_iterator;
	phiz_ce_Str8->ce_flags |= ZEND_ACC_REUSE_GET_ITERATOR;
}


