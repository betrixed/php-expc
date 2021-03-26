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
#include "php_phiz.h"
#include "Zend/zend_interfaces.h"
#define phiz_ce_Countable     zend_ce_countable
#define phiz_ce_ArrayAccess	  zend_ce_arrayaccess
#define phiz_ce_Iterator	  zend_ce_iterator
#define phiz_ce_Aggregate     zend_ce_aggregate




//#define Z_PHIZ_CARRAY_P(zv)  phiz_carray_from_obj(Z_OBJ_P((zv)))

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
	// no return value
}


PHP_METHOD(CArray, resize)
{
	zend_long 		size;
	pz_carray 		intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	pca_resize(&intern->cobj, size);
	RETURN_TRUE;

}

PHP_METHOD(CArray, reserve)
{
	zend_long 		size;
	pz_carray 		intern;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	pca_reserve(&intern->cobj, size);
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


PHP_METHOD(CArray, getTypeEnum)
{
	pz_carray 		intern;


	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	RETURN_LONG(intern->cobj.fntab->etype);

}

PHP_METHOD(CArray, getTypeName)
{
	pz_carray 		intern;


	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	ZVAL_STRING(return_value, intern->cobj.fntab->ename);

}


PHP_METHOD(CArray, signedType)
{
	pz_carray 		intern;


	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	if ( ((intern->cobj.fntab->etype) & 0x01) == 0) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

PHP_METHOD(CArray, size)
{
	pz_carray 		intern;
	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	RETURN_LONG(intern->cobj.size);
}

PHP_METHOD(CArray, capacity)
{
	pz_carray 		intern;
	ZEND_PARSE_PARAMETERS_NONE();

	intern = Z_PHIZ_CARRAY_P(ZEND_THIS);

	RETURN_LONG(intern->cobj.capacity);
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

static int carray_obj_has_dimension(zend_object *object, zval *offset, int check_empty)
{
	pz_carray intern = phiz_carray_from_obj(object);
	zend_long index;

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

	if (Z_TYPE_P(offset) != IS_LONG) {
		index = phiz_offset_convert_to_long(offset);
	} else {
		index = Z_LVAL_P(offset);
	}

	if (index < 0 || index >= intern->cobj.size) {
		return 0;
	} else {
		// *EMPTY* values are not implemented
		return  1;
	}
}

static zval *carray_obj_read_dimension(
	zend_object *object, zval *offset, int type, zval *rv)
{
	pz_carray intern = phiz_carray_from_obj(object);
	zend_long index;
	p_carray_obj pobj;

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
		pobj = &intern->cobj;
		return pobj->fntab->get_zval(pobj, index, offset);
	}
}

static void carray_obj_write_dimension_helper(pz_carray intern, zval *offset, zval *value)
{
	zend_long index;
	p_carray_obj pobj;
	int  set_error = CATE_TYPE;

	if (!offset) {
		/* '$array[] = value' syntax is not supported */

		pobj = &intern->cobj;
		set_error = pca_pushback(pobj, value);
	}
	else {
		if (Z_TYPE_P(offset) != IS_LONG) {
			index = phiz_offset_convert_to_long(offset);
		} else {
			index = Z_LVAL_P(offset);
		}

		if (index < 0 || index >= intern->cobj.size) {
			zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
			return;
		} else {
			pobj = &intern->cobj;
			set_error = pobj->fntab->set_zval(pobj, index, value);
		}
	}
	if (set_error != CATE_OK) {
		//printf(" SET ERROR = %d", set_error);
		zend_throw_exception_ex(phiz_ce_RuntimeException, 0,
			"Value outside storage range for %s %Z", pobj->fntab->ename, value);
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
		p_carray_obj pobj = &intern->cobj;
		carray_obj_fntab* fntab = pobj->fntab;
		if (fntab->ctdt) {
			fntab->dtor_elems(pobj,index,index+1);
		}
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

/* Create a new iterator from a SplFixedArray instance. */
PHP_METHOD(CArray, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}



zend_object_iterator *carray_get_iterator(
	zend_class_entry *ce, zval *object, int by_ref)
{
	carray_quickit *iterator;

	if (by_ref) {
		zend_throw_error(NULL, "An iterator cannot be used with foreach by reference");
		return NULL;
	}

	iterator = emalloc(sizeof(carray_quickit));

	zend_iterator_init((zend_object_iterator*)iterator);
	ZVAL_UNDEF(&iterator->result);
	ZVAL_OBJ_COPY(&iterator->intern.data, Z_OBJ_P(object));
	pz_carray intern = Z_PHIZ_CARRAY_P(object);
	p_carray_obj pobj = &intern->cobj;

	iterator->intern.funcs = &pobj->fntab->zit_funcs;

	return &iterator->intern;
}

PHP_MINIT_FUNCTION(phiz_carray)
{
	phiz_register_std_class(&phiz_ce_CArray, "CArray", 
		phiz_carray_new, class_CArray_methods);
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
	//REGISTER_PHIZ_CLASS_CONST_LONG("CA_UINT64", (zend_long)CAT_UINT64);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_REAL32", (zend_long)CAT_REAL32);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_REAL64", (zend_long)CAT_REAL64);
	REGISTER_PHIZ_CLASS_CONST_LONG("CA_MIXED", (zend_long)CAT_MIXED);

	REGISTER_PHIZ_IMPLEMENTS(CArray, Aggregate);
	REGISTER_PHIZ_IMPLEMENTS(CArray, ArrayAccess);
	REGISTER_PHIZ_IMPLEMENTS(CArray, Countable);
	

	phiz_ce_CArray->get_iterator = carray_get_iterator;
	phiz_ce_CArray->ce_flags |= ZEND_ACC_REUSE_GET_ITERATOR;

	return SUCCESS;
}