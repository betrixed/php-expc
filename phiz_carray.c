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