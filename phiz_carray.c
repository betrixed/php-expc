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
	carray_obj           cobj;		// embedded polymorphic array
	long 		  	     current; // 0 - indexed offset
	//-- these ackward complicating function pointers 
	//-- are here for the case when a derived class object implements an override method
	zend_function       *fptr_offset_get;
	zend_function       *fptr_offset_set;
	zend_function       *fptr_offset_has;
	zend_function       *fptr_offset_del;
	zend_function       *fptr_count;
	//-- standard object
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

	if (inherited) {
		intern->fptr_offset_get = zend_hash_str_find_ptr(&class_type->function_table, "offsetget", sizeof("offsetget") - 1);
		if (intern->fptr_offset_get->common.scope == parent) {
			intern->fptr_offset_get = NULL;
		}
		intern->fptr_offset_set = zend_hash_str_find_ptr(&class_type->function_table, "offsetset", sizeof("offsetset") - 1);
		if (intern->fptr_offset_set->common.scope == parent) {
			intern->fptr_offset_set = NULL;
		}
		intern->fptr_offset_has = zend_hash_str_find_ptr(&class_type->function_table, "offsetexists", sizeof("offsetexists") - 1);
		if (intern->fptr_offset_has->common.scope == parent) {
			intern->fptr_offset_has = NULL;
		}
		intern->fptr_offset_del = zend_hash_str_find_ptr(&class_type->function_table, "offsetunset", sizeof("offsetunset") - 1);
		if (intern->fptr_offset_del->common.scope == parent) {
			intern->fptr_offset_del = NULL;
		}
		intern->fptr_count = zend_hash_str_find_ptr(&class_type->function_table, "count", sizeof("count") - 1);
		if (intern->fptr_count->common.scope == parent) {
			intern->fptr_count = NULL;
		}

	}

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

// direct steal from spl_offset_convert_to_long
PHPAPI zend_long phiz_offset_convert_to_long(zval *offset) /* {{{ */
{
	zend_ulong idx;

try_again:
	switch (Z_TYPE_P(offset)) {
	case IS_STRING:
		if (ZEND_HANDLE_NUMERIC(Z_STR_P(offset), idx)) {
			return idx;
		}
		break;
	case IS_DOUBLE:
		return zend_dval_to_lval(Z_DVAL_P(offset));
	case IS_LONG:
		return Z_LVAL_P(offset);
	case IS_FALSE:
		return 0;
	case IS_TRUE:
		return 1;
	case IS_REFERENCE:
		offset = Z_REFVAL_P(offset);
		goto try_again;
	case IS_RESOURCE:
		return Z_RES_HANDLE_P(offset);
	}
	return -1;
}

static zval *carray_obj_read_dimension_helper(pz_carray intern, zval *offset)
{
	zend_long index;

	/* we have to return NULL on error here to avoid memleak because of
	 * ZE duplicating uninitialized_zval_ptr */
	if (!offset) {
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		return NULL;
	}

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = phiz_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	
	if (index < 0 || index >= intern->cobj.size) {
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		return NULL;
	} else {
		// reuse of zval* offset is only option here
		intern->cobj.fntab->get_zval(&intern->cobj, index, offset);
		return offset;
	}
}

static int carray_obj_has_dimension_helper(pz_carray intern, zval *offset, int check_empty)
{
	zend_long index;
	int retval;

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = phiz_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || index >= intern->cobj.size) {
		retval = 0;
	} else {
		// *EMPTY* values are not implemented
		return  1;
		/*if (check_empty) {
			retval = zend_is_true(&intern->array.elements[index]);
		} else {
			retval = Z_TYPE(intern->array.elements[index]) != IS_NULL;
		}
		*/
	}

	return retval;
}

static int carray_obj_has_dimension(zend_object *object, zval *offset, int check_empty)
{
	pz_carray intern = phiz_carray_from_obj(object);

	if (intern->fptr_offset_has) {
		zval rv;
		zend_bool result;

		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(object, intern->std.ce, &intern->fptr_offset_has, "offsetExists", &rv, offset);
		zval_ptr_dtor(offset);
		result = zend_is_true(&rv);
		zval_ptr_dtor(&rv);
		return result;
	}

	return carray_obj_has_dimension_helper(intern, offset, check_empty);
}

static zval *carray_obj_read_dimension(
	zend_object *object, zval *offset, int type, zval *rv)
{
	pz_carray intern = phiz_carray_from_obj(object);

	if (type == BP_VAR_IS && !carray_obj_has_dimension(object, offset, 0)) {
		return &EG(uninitialized_zval);
	}

	if (intern->fptr_offset_get) {
		zval tmp;
		if (!offset) {
			ZVAL_NULL(&tmp);
			offset = &tmp;
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		zend_call_method_with_1_params(object, intern->std.ce, &intern->fptr_offset_get, "offsetGet", rv, offset);
		zval_ptr_dtor(offset);
		if (!Z_ISUNDEF_P(rv)) {
			return rv;
		}
		return &EG(uninitialized_zval);
	}

	return carray_obj_read_dimension_helper(intern, offset);
}

static void carray_obj_write_dimension_helper(pz_carray intern, zval *offset, zval *value)
{
	zend_long index;

	if (!offset) {
		/* '$array[] = value' syntax is not supported */
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		return;
	}

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = phiz_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || index >= intern->cobj.size) {
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		return;
	} else {
		intern->cobj.fntab->set_zval(&intern->cobj, index, value);
	}
}

static void carray_obj_write_dimension(zend_object *object, zval *offset, zval *value)
{
	zval tmp;
	pz_carray intern = phiz_carray_from_obj(object);

	if (intern->fptr_offset_set) {
		if (!offset) {
			ZVAL_NULL(&tmp);
			offset = &tmp;
		} else {
			SEPARATE_ARG_IF_REF(offset);
		}
		SEPARATE_ARG_IF_REF(value);
		zend_call_method_with_2_params(object, intern->std.ce, &intern->fptr_offset_set, "offsetSet", NULL, offset, value);
		zval_ptr_dtor(value);
		zval_ptr_dtor(offset);
		return;
	}

	carray_obj_write_dimension_helper(intern, offset, value);
}

static void carray_obj_unset_dimension_helper(pz_carray intern, zval *offset)
{
	zend_long index;

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = phiz_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || index >= intern->cobj.size) {
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		return;
	} else {
		/* NO EMPTY value token to set yet*/
	}
}

static void carray_obj_unset_dimension(zend_object *object, zval *offset)
{
	pz_carray intern = phiz_carray_from_obj(object);

	if (intern->fptr_offset_del) {
		SEPARATE_ARG_IF_REF(offset);
		zend_call_method_with_1_params(object, intern->std.ce, &intern->fptr_offset_del, "offsetUnset", NULL, offset);
		zval_ptr_dtor(offset);
		return;
	}

	carray_obj_unset_dimension_helper(intern, offset);
}

static int carray_obj_count_elements(zend_object *object, zend_long *count)
{
	pz_carray intern = phiz_carray_from_obj(object);
	if (intern->fptr_count) {
		zval rv;
		zend_call_method_with_0_params(object, intern->std.ce, &intern->fptr_count, "count", &rv);
		if (!Z_ISUNDEF(rv)) {
			*count = zval_get_long(&rv);
			zval_ptr_dtor(&rv);
		} else {
			*count = 0;
		}
	} else {
		*count = intern->cobj.size;
	}
	return SUCCESS;
}

PHP_MINIT_FUNCTION(phiz_carray)
{
	phiz_register_std_class(&phiz_ce_CArray, "CArray", phiz_carray_new, class_CArray_methods);
	memcpy(&phiz_handler_CArray, &std_object_handlers, sizeof(zend_object_handlers));

	phiz_handler_CArray.offset = XtOffsetOf(phiz_carray_obj, std);
	phiz_handler_CArray.clone_obj = phiz_carray_clone;
	phiz_handler_CArray.dtor_obj  = zend_objects_destroy_object;
	phiz_handler_CArray.free_obj  = phiz_carray_free_storage;

	phiz_handler_CArray.read_dimension  = carray_obj_read_dimension;
	phiz_handler_CArray.write_dimension = carray_obj_write_dimension;
	phiz_handler_CArray.unset_dimension = carray_obj_unset_dimension;
	phiz_handler_CArray.has_dimension   = carray_obj_has_dimension;
	phiz_handler_CArray.count_elements  = carray_obj_count_elements;

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