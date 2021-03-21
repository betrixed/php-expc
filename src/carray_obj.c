
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "carray_obj.h"



static void carray_default_ctor(p_carray_obj array)
{
	array->size = 0;
	array->elements = NULL;
}

void pca_alloc(p_carray_obj this, long size) {
	if (size > 0) {
		this->size = 0; /* reset size in case ecalloc() fails */
		this->elements = safe_emalloc(size, this->fntab->esize, 0);
		this->size = size;
	}
}

bool pca_is_empty(p_carray_obj this) {
	if (this->elements) {
		ZEND_ASSERT(this->size > 0);
		return false;
	}
	ZEND_ASSERT(this->size == 0);
	return true;
}

void pca_free(p_carray_obj this)
{
	if (!pca_is_empty(this)) {
		efree(this->elements);
		this->elements = NULL;
		this->size = 0;
	}
}

void pca_resize(p_carray_obj this, long size) {

	if (this->size == size) {
		return;
	}

	if (this->size == 0) {
		pca_alloc(this,size);
		this->fntab->init_elems(this,0,size);
		return;
	}
	if (size == 0) {
		efree(this->elements);
		this->elements = NULL;
		this->size = 0;
		return;
	}
	carray_obj_fntab* fntab = this->fntab;
	if (size > this->size) {
		this->elements = safe_erealloc(this->elements, size, fntab->esize, 0);
		fntab->init_elems(this,this->size,size);
		this->size = size;
		return;
	}	
	this->elements = erealloc(this->elements, size * fntab->esize);
	this->size = size;
}



// CAT_INT8
#define PHIZ_CARRAY_ETYPE CAT_INT8
#define PHIZ_CAST_TYPE int8_t
#define PHIZ_CAST_NAME int8
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_CAST_MIN CHAR_MIN
#define PHIZ_CAST_MAX CHAR_MAX
#include "cast_macro.c"

// CAT_UINT8
#define PHIZ_CARRAY_ETYPE CAT_UINT8
#define PHIZ_CAST_TYPE uint8_t
#define PHIZ_CAST_NAME uint8
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_CAST_MIN 0
#define PHIZ_CAST_MAX UINT8_MAX
#include "cast_macro.c"

// CAT_INT16
#define PHIZ_CARRAY_ETYPE CAT_INT16
#define PHIZ_CAST_TYPE  int16_t
#define PHIZ_CAST_NAME  int16
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_CAST_MIN INT16_MIN
#define PHIZ_CAST_MAX INT16_MAX
#include "cast_macro.c"


// CAT_UINT16
#define PHIZ_CARRAY_ETYPE CAT_UINT16
#define PHIZ_CAST_TYPE  uint16_t
#define PHIZ_CAST_NAME  uint16
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_CAST_MIN 0
#define PHIZ_CAST_MAX UINT16_MAX
#include "cast_macro.c"



// CAT_INT32
#define PHIZ_CARRAY_ETYPE CAT_INT32
#define PHIZ_CAST_TYPE int32_t
#define PHIZ_CAST_NAME int32
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_CAST_MIN INT32_MIN
#define PHIZ_CAST_MAX INT32_MAX
#include "cast_macro.c"

// CAT_UINT32
#define PHIZ_CARRAY_ETYPE CAT_UINT32
#define PHIZ_CAST_TYPE uint32_t
#define PHIZ_CAST_NAME uint32
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_CAST_MIN 0
#define PHIZ_CAST_MAX UINT32_MAX
#include "cast_macro.c"

// CAT_INT64
#define PHIZ_CARRAY_ETYPE CAT_INT64
#define PHIZ_CAST_TYPE int64_t
#define PHIZ_CAST_NAME int64
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_CAST_MIN INT64_MIN
#define PHIZ_CAST_MAX INT64_MAX
#include "cast_macro.c"

// CAT_UINT64
#define PHIZ_CARRAY_ETYPE CAT_UINT64
#define PHIZ_CAST_TYPE uint64_t
#define PHIZ_CAST_NAME uint64
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_CAST_MIN 0
#define PHIZ_CAST_MAX UINT64_MAX
#include "cast_macro.c"

// CAT_REAL32
#define PHIZ_CARRAY_ETYPE CAT_REAL32
#define PHIZ_CAST_TYPE float
#define PHIZ_CAST_NAME float
#define PHIZ_ZVAL_TYPE DOUBLE
#define PHIZ_ZVAL_CAST_TYPE Z_DVAL_P
#define PHIZ_CAST_MIN -FLT_MAX
#define PHIZ_CAST_MAX FLT_MAX
#include "cast_macro.c"

// CAT_REAL64

#define PHIZ_CARRAY_ETYPE CAT_REAL64
#define PHIZ_CAST_TYPE double
#define PHIZ_CAST_NAME double
#define PHIZ_ZVAL_TYPE DOUBLE
#define PHIZ_ZVAL_CAST_TYPE Z_DVAL_P
#define PHIZ_CAST_MIN -DBL_MAX
#define PHIZ_CAST_MAX DBL_MAX
#include "cast_macro.c"


void carray_etype_ctor(p_carray_obj this, int etype) {
	switch(etype) {
		case CAT_INT8:
			this->fntab = &int8_fntab;
			break;
		case CAT_UINT8:
			this->fntab = &uint8_fntab;
			break;
		case CAT_INT16:
			this->fntab = &int16_fntab;
			break;
		case CAT_UINT16:
			this->fntab = &uint16_fntab;
			break;
		case CAT_INT32: 
			this->fntab = &int32_fntab;
			break;
		case CAT_UINT32: 
			this->fntab = &uint32_fntab;
			break;
		case CAT_INT64: 
			this->fntab = &int64_fntab;
			break;
		case CAT_UINT64: 
			this->fntab = &uint64_fntab;
			break;
		case CAT_REAL32: 
			this->fntab = &float_fntab;
			break;
		case CAT_REAL64: 
			this->fntab = &double_fntab;
			break;
	}
	this->elements = NULL;
	this->size = 0;
}

void carray_copy_ctor(p_carray_obj this, p_carray_obj from)
{
	carray_etype_ctor(this, from->fntab->etype);
	int size = from->size;
	if (size > 0) {
		pca_alloc(this,from->size);
		(this->fntab)->copy_elems(this,0,from,0,from->size);
	}
}


