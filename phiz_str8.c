#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "php.h"
#include "phiz_str8.h"
#include "phiz_str8_arginfo.h"
#include "Zend/zend_interfaces.h"
#include "src/ucode8.h"

typedef struct _phiz_str8_obj {
	
	long 		  current; // 0 - indexed offset
	long          next;	   // 0 - indexed offset
	long          ucode; // holds char32_t value
	zval 		  target;
	zend_object   std;
} phiz_str8_obj;

static inline phiz_str8_obj *phiz_str8_from_obj(zend_object *obj) /* {{{ */ {
	return (phiz_str8_obj*)((char*)(obj) - XtOffsetOf(phiz_str8_obj, std));
}

#define Z_PHIZ_STR8_P(zv)  phiz_str8_from_obj( Z_OBJ_P((zv)))

// structure for pointer to functions
zend_object_handlers 	phiz_handler_Str8;

PHPAPI zend_class_entry *phiz_ce_Str8;

void phiz_str8_init(phiz_str8_obj* this) {
    this->current = -1;
	this->next = 0;
	this->ucode = INVALID_CHAR;
}

bool phiz_str8_advance(phiz_str8_obj* this) {
	zend_string* s = Z_STR(this->target);

	long next = this->next;
	long slen = ZSTR_LEN(s) - next;

	if (slen > 0) {
		char32_t value = INVALID_CHAR;
		long units = ucode8Fore(ZSTR_VAL(s) + next, slen, &value);

		if (units > 0 && value != INVALID_CHAR) {
			this->current = next;
			this->next = next + units;
			this->ucode = value;
			return true;
		}
	}
	this->current = this->next;
	this->ucode = INVALID_CHAR;
	return false;
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

	phiz_str8_init(intern);
	phiz_str8_advance(intern);
}

/* {{{ Return the iterated string */
PHP_METHOD(Str8, iteratee)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;
	zend_string *target;

	intern = Z_PHIZ_STR8_P(object);
	
	ZVAL_COPY_VALUE(return_value, &intern->target);
}

/* {{{ Returns state of iteration */
PHP_METHOD(Str8, valid)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;

	intern = Z_PHIZ_STR8_P(object);
	if (intern->next == intern->current) {
		// empty
		RETVAL_FALSE;
	}
	else {
		RETVAL_TRUE;
	}
}

/* {{{ Returns nothing (undefined), update internals */
PHP_METHOD(Str8, next)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;

	intern = Z_PHIZ_STR8_P(object);
	phiz_str8_advance(intern);
}

/* {{{ Returns nothing (undefined), update internals */
PHP_METHOD(Str8, rewind)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;

	intern = Z_PHIZ_STR8_P(object);
	phiz_str8_init(intern);
	phiz_str8_advance(intern);
}
/* {{{ Return 8-bit character offset as longint */
PHP_METHOD(Str8, key)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;
	intern = Z_PHIZ_STR8_P(object);
	ZVAL_LONG(return_value, intern->current);
}

/* {{{ Return a 32 bit unicode value as zval longint */
PHP_METHOD(Str8, current)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;

	intern = Z_PHIZ_STR8_P(object);

	ZVAL_LONG(return_value, intern->ucode);

}

/* {{{ Return a 32 bit unicode value as zval longint */
PHP_METHOD(Str8, byteslen)
{
	zval *object = ZEND_THIS;
	phiz_str8_obj* intern;

	intern = Z_PHIZ_STR8_P(object);

	ZVAL_LONG(return_value, intern->next - intern->current);

}

/* {{{ Return a 32 bit unicode value as zval longint */
PHP_METHOD(Str8, bytes)
{
	zval *object = ZEND_THIS;

	phiz_str8_obj* intern = Z_PHIZ_STR8_P(object);

	zend_string* s = Z_STR(intern->target);
	long offset = intern->current;
	long slen = intern->next - intern->current;

	ZVAL_STRINGL(return_value, ZSTR_VAL(s) + offset, slen);
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


PHP_MINIT_FUNCTION(phiz_str8)
{
	phiz_ce_Str8 = register_class_Str8(zend_ce_iterator);
	phiz_ce_Str8->create_object  = phiz_str8_new;
	memcpy(&phiz_handler_Str8, &std_object_handlers, sizeof(zend_object_handlers));
	phiz_handler_Str8.offset = XtOffsetOf(phiz_str8_obj, std);

}
static void phiz_str8_free_storage(zend_object *object)
{
	phiz_str8_obj* intern = phiz_str8_from_obj(object);

	Z_TRY_DELREF(intern->target);
	zend_object_std_dtor(&intern->std);
}
