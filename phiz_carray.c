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

#include "Zend/zend_interfaces.h"
#define phiz_ce_Countable     zend_ce_countable
#define phiz_ce_ArrayAccess	  zend_ce_arrayaccess
#define phiz_ce_Iterator	  zend_ce_iterator



typedef struct _phiz_carray_obj* pz_carray;

typedef struct _phiz_carray_obj {
	carray_obj           cobj;
	long 		  	     current; // 0 - indexed offset
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

	// presume not initialized
	carray_etype_ctor(&intern->cobj, etype);
	if (size > 0) {
		pca_alloc(&intern->cobj, size);
		intern->cobj.fntab->init_elems(&intern->cobj, 0, size);
	}
	intern->current = 0;
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

	pca_resize(&intern->cobj, size);
	intern->current = 0;
	RETURN_TRUE;

}

PHP_METHOD(CArray, getType)
{
	pz_carray 		intern;
	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	RETURN_LONG(intern->cobj.fntab->etype);
}

PHP_METHOD(CArray, getTypeSize)
{
	pz_carray 		intern;
	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	RETURN_LONG(intern->cobj.fntab->esize);
}

PHP_METHOD(CArray, getTypeName)
{
	pz_carray 		intern;


	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	ZVAL_STRING(return_value, intern->cobj.fntab->ename);

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
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		RETURN_NULL();
	}
}

/* Sets the value at the specified $index to $newval. */
PHP_METHOD(CArray, offsetSet)
{
	zend_long		zindex;
	pz_carray 		intern;
	zval*            value;
	int             set_error;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(zindex)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);
	p_carray_obj pobj = &intern->cobj;
	carray_obj_fntab* fntab = pobj->fntab;

	if ((zindex >= 0) && (zindex < pobj->size)) {	
		set_error = pobj->fntab->set_zval(pobj, zindex, value);
		if (set_error != CATE_OK) {
			zend_throw_exception(phiz_ce_RuntimeException, "Element value outside range", 0);
		}
	}
	else {
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
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

/* {{{ Returns state of iteration */
PHP_METHOD(CArray, valid)
{
	pz_carray  intern;

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);
	if (intern->current < intern->cobj.size) {
		// empty
		RETVAL_TRUE;
	}
	else {
		RETVAL_FALSE;
	}
}


/* {{{ Returns nothing (undefined), update internals */
PHP_METHOD(CArray, next)
{
	pz_carray  intern = Z_PHIZ_CARRAY_P(ZEND_THIS);
	intern->current++;
}

/* {{{ Returns nothing (undefined), update internals */
PHP_METHOD(CArray, rewind)
{
	pz_carray  intern = Z_PHIZ_CARRAY_P(ZEND_THIS);
	intern->current = 0;
}


/* {{{ Return a zval longint */
PHP_METHOD(CArray, key)
{
	pz_carray  intern = Z_PHIZ_CARRAY_P(ZEND_THIS);
	ZVAL_LONG(return_value, intern->current);
}

/* {{{ Return zval as value */
PHP_METHOD(CArray, current)
{
	pz_carray  intern = Z_PHIZ_CARRAY_P(ZEND_THIS);
	p_carray_obj pobj = &intern->cobj;

	if ((intern->current >= 0) && (intern->current < pobj->size)) {
		pobj->fntab->get_zval(pobj, intern->current, return_value);
	}
	else {
		zend_throw_exception(phiz_ce_RuntimeException, "Iterator index out of range", 0);
		RETURN_NULL();
	}

}

static void phiz_carray_ctor(p_carray_obj this, p_carray_obj from)
{

	carray_etype_ctor(this, from->fntab->etype);

	size_t size = from->size;
	if (size > 0) {
		pca_alloc(this,size);
	}
}

static void phiz_carray_copy_ctor( p_carray_obj to, p_carray_obj from)
{
	
	carray_copy_ctor(to, from);

}

// not implementing inherited (yet)
static zend_object *phiz_carray_new_ex(zend_class_entry *class_type,
									zend_object *orig, bool clone_orig)
{
	pz_carray 		  	   intern;
	zend_class_entry      *parent = class_type;
	bool                   inherited = false;

	intern = zend_object_alloc(sizeof(phiz_carray_obj), class_type);

	zend_object_std_init(&intern->std, class_type);

	object_properties_init(&intern->std, class_type);

	if (orig && clone_orig) {
		pz_carray other = phiz_carray_from_obj(orig);
		phiz_carray_copy_ctor(&intern->cobj, &other->cobj);
	}
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


static void phiz_carray_free_storage(zend_object *object)
{
	pz_carray intern = phiz_carray_from_obj(object);
	pca_free(&intern->cobj);
	zend_object_std_dtor(&intern->std);
}

static zend_object *phiz_carray_clone(zend_object *old_object)
{
	zend_object *new_object = phiz_carray_new_ex(old_object->ce, old_object, 1);

	zend_objects_clone_members(new_object, old_object);

	return new_object;
}


PHPAPI void phiz_register_std_class(zend_class_entry ** ppce, char * class_name, void * obj_ctor, const zend_function_entry * function_list)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY_EX(ce, class_name, strlen(class_name), function_list);
	*ppce = zend_register_internal_class(&ce);

	/* entries changed by initialize */
	if (obj_ctor) {
		(*ppce)->create_object = obj_ctor;
	}
}




PHP_MINIT_FUNCTION(phiz_carray)
{
	phiz_register_std_class(&phiz_ce_CArray, "CArray", phiz_carray_new, class_CArray_methods);
	memcpy(&phiz_handler_CArray, &std_object_handlers, sizeof(zend_object_handlers));

	phiz_handler_CArray.offset = XtOffsetOf(phiz_carray_obj, std);
	phiz_handler_CArray.clone_obj = phiz_carray_clone;
	phiz_handler_CArray.dtor_obj  = zend_objects_destroy_object;
	phiz_handler_CArray.free_obj  = phiz_carray_free_storage;

	REGISTER_PHIZ_CLASS_CONST_LONG("CA_INT8", (zend_long)CAT_INT8);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_UINT8", (zend_long)CAT_UINT8);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_INT16", (zend_long)CAT_INT16);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_UINT16", (zend_long)CAT_UINT16);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_INT32", (zend_long)CAT_INT32);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_UINT32", (zend_long)CAT_UINT32);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_INT64", (zend_long)CAT_INT64);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_UINT64", (zend_long)CAT_UINT64);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_REAL32", (zend_long)CAT_REAL32);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_REAL64", (zend_long)CAT_REAL64);
	//REGISTER_PHIZ_IMPLEMENTS(CArray, Aggregate);

	REGISTER_PHIZ_IMPLEMENTS(CArray, ArrayAccess);
	REGISTER_PHIZ_IMPLEMENTS(CArray, Countable);
	REGISTER_PHIZ_IMPLEMENTS(CArray, Iterator);
	return SUCCESS;
}