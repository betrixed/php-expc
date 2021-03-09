#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"

#include "phiz_carray_arginfo.h"
#include "phiz_carray.h"
#include "src/carray_obj.h"


typedef struct _phiz_carray_obj* pz_carray;

typedef struct _phiz_carray_obj {
	carray_obj           cobj;
	zend_object          std;
} phiz_carray_obj;

static  pz_carray phiz_carray_from_obj(zend_object *obj)
{
	return (pz_carray)((char*)(obj) - XtOffsetOf(phiz_carray_obj, std));
}

#define Z_PHIZ_CARRAY_P(zv)  phiz_carray_from_obj(Z_OBJ_P((zv)))

// structure for pointer to functions
zend_object_handlers 	phiz_handler_CArray;

PHPAPI zend_class_entry *phiz_ce_CArray;

/* Helps enforce the invariants in debug mode:
 *   - if size == 0, then elements == NULL
 *   - if size > 0, then elements != NULL
 *   - size is not less than 0
 */
static bool phiz_carray_empty(p_carray_obj array)
{
	if (array->elements) {
		ZEND_ASSERT(array->size > 0);
		return false;
	}
	ZEND_ASSERT(array->size == 0);
	return true;
}


PHP_METHOD(CArray, __construct)
{
	zval *object = ZEND_THIS;
	pz_carray 	   intern;
	zend_long	   etype;
	zend_long 	   size = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(etype)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CARRAY_P(object);

	// presume not initilized
	carray_ctor_etype(&intern->cobj, etype);
	if (size > 0) {
		intern->cobj.fntab->alloc_size(&intern->cobj, size);
		intern->cobj.fntab->init_elems(&intern->cobj, 0, size);
	}
	// no return value
}


PHP_METHOD(CArray, setSize)
{
	zend_long 		size;
	pz_carray 		intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	intern->cobj.fntab->resize(&intern->cobj, size);
	RETURN_TRUE;

}
PHP_METHOD(CArray, getSize)
{
	pz_carray 		intern;
	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	RETURN_LONG(intern->cobj.size);
}

PHP_METHOD(CArray, count)
{
	pz_carray 		intern;
	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	RETURN_LONG(intern->cobj.size);
}

PHP_METHOD(CArray, toArray)
{
	pz_carray intern;
	zval      item;

	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	ptrdiff_t size = (ptrdiff_t) intern->cobj.size;
	if (size > 0) {
		array_init(return_value);
		
		p_carray_obj pobj = &intern->cobj;
		carray_obj_fntab* fntab = pobj->fntab;

		for (zend_long i = 0; i < size; i++) {
			fntab->get_zval(pobj, i, &item);
			zend_hash_index_update(Z_ARRVAL_P(return_value), i, &item);
		}
	} else {
		RETURN_EMPTY_ARRAY();
	}
}

/* Returns the value at the specified $index. */
PHP_METHOD(CArray, offsetGet)
{
	zend_long		zindex;
	pz_carray 		intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(zindex)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);
	
	p_carray_obj pobj = &intern->cobj;
	carray_obj_fntab* fntab = pobj->fntab;

	if ((zindex >= 0) && (zindex < pobj->size)) {
		fntab->get_zval(pobj, zindex, return_value);
	}
	else {
		// TODO: throw OutOfRange 
		RETURN_NULL();
	}
}

/* Sets the value at the specified $index to $newval. */
PHP_METHOD(CArray, offsetSet)
{
	zend_long		zindex;
	pz_carray 		intern;
	zval*            value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(zindex)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	p_carray_obj pobj = &intern->cobj;
	carray_obj_fntab* fntab = pobj->fntab;

	if ((zindex >= 0) && (zindex < pobj->size)) {	
		pobj->fntab->set_zval(pobj, zindex, value);
	}
	else {
		// TODO: throw OutOfRange 
	}
}

PHP_METHOD(CArray, offsetExists)
{
	zend_long		zindex;
	pz_carray 		intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(zindex)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	RETURN_BOOL((zindex >= 0 && zindex < intern->cobj.size));
}

/* Unsets the value at the specified $index. */
PHP_METHOD(CArray, offsetUnset)
{
	zend_long		zindex;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(zindex)
	ZEND_PARSE_PARAMETERS_END();
	// Not implemented
}

static void phiz_carray_ctor(p_carray_obj this, p_carray_obj from)
{

	carray_ctor_etype(this, from->fntab->etype);

	size_t size = from->size;
	if (size > 0) {
		(this->fntab)->alloc_size(this,size);
	}
}

static void phiz_carray_copy_ctor( p_carray_obj to, p_carray_obj from)
{
	
	carray_ctor_copy(to, from);

}

// not implementing inherited (yet)
static zend_object *phiz_carray_new_ex(zend_class_entry *class_type,
									zend_object *orig, bool clone_orig)
{
	pz_carray_obj 		  intern;
	zend_class_entry      *parent = class_type;
	bool                   inherited = false;

	intern = zend_object_alloc(sizeof(phiz_carray_obj), class_type);
	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	while(parent) {
		if (parent == phiz_ce_CArray) {
			intern->std.handlers = &phiz_handler_CArray;
			break;
		}
		parent = parent->parent;
		inherited = true;
	}

	ZEND_ASSERT(parent);

	ZEND_ASSERT(!inherited);

	return &intern->std;
	
}
static zend_object *phiz_carray_new(zend_class_entry *class_type)
{
	return phiz_carray_new_ex(class_type, NULL, 0);
}

PHP_MINIT_FUNCTION(phiz_carray)
{
	phiz_ce_CArray = register_class_CArray(zend_ce_iterator);
	phiz_ce_CArray->create_object  = phiz_carray_new;
	memcpy(&phiz_handler_CArray, &std_object_handlers, sizeof(zend_object_handlers));
	phiz_handler_CArray.offset = XtOffsetOf(phiz_carray_obj, std);

}