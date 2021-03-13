
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

static void pca_initelems_uint32(p_carray_obj this, long from, long to) {
	ZEND_ASSERT(from <= to);
	ZEND_ASSERT(to <= this->size);

	uint32_t* begin = (uint32_t*) this->elements + from;
	uint32_t* end = (uint32_t*) this->elements + to;
	while (begin != end) {
		*begin++ = 0;
	}
}

static void pca_initelems_int8(p_carray_obj this, long from, long to) {

	ZEND_ASSERT(from <= to);
	ZEND_ASSERT(to <= this->size);

	int8_t* begin = (int8_t*) this->elements + from;
	int8_t* end = (int8_t*) this->elements + to;
	while (begin != end) {
		*begin++ = 0;
	}
}


static void pca_copyelems_uint32( p_carray_obj this, zend_long offset, 
	p_carray_obj other, zend_long begin, zend_long end)
{
	ZEND_ASSERT(offset >= 0);
	ZEND_ASSERT(this->size - offset >= end - begin);

	uint32_t *to = (uint32_t*) this->elements + offset;
	uint32_t *bgp = (uint32_t*) other->elements + begin;
	uint32_t *ep = (uint32_t*) other->elements + end;

	while (bgp != ep) {
		*to++ = *bgp++;
	}
}

static void pca_getzval_int8( p_carray_obj this, zend_long offset,  zval* retval)
{
	zend_long value = *((int8_t*) this->elements + offset);
	ZVAL_LONG(retval,value);
}

static void pca_getzval_uint32( p_carray_obj this, zend_long offset,  zval* retval)
{
	zend_long value = *((uint32_t*) this->elements + offset);
	ZVAL_LONG(retval,value);
}

static int pca_setzval_int8( p_carray_obj this, zend_long offset,  zval* setval)
{
	if (Z_TYPE_P(setval) != IS_LONG){
		return CATE_TYPE;
	}
	zend_long val = Z_LVAL_P(setval);
	if (val < CHAR_MIN || val > CHAR_MAX) {
		return CATE_RANGE;
	}
	*((int8_t*) this->elements + offset) = val;
	return CATE_OK;
}

static int pca_setzval_uint32( p_carray_obj this, zend_long offset,  zval* setval)
{
	if (Z_TYPE_P(setval) != IS_LONG){
		return CATE_TYPE;
	}
	zend_long val = Z_LVAL_P(setval);
	if (val < 0 || val > UINT_MAX) {
		return  CATE_RANGE;
	}
	*((uint32_t*) this->elements + offset) = val;
	return CATE_OK;
}

static void pca_copyelems_int8( p_carray_obj this, zend_long offset, 
	p_carray_obj other, zend_long begin, zend_long end)
{
	ZEND_ASSERT(offset >= 0);
	ZEND_ASSERT(this->size - offset >= end - begin);

	int8_t *to = (int8_t*) this->elements + offset;
	int8_t *bgp = (int8_t*) other->elements + begin;
	int8_t *ep = (int8_t*) other->elements + end;

	while (bgp != ep) {
		*to++ = *bgp++;
	}
}



static carray_obj_fntab int8_fntab = {
	CAT_INT8,
	sizeof(int8_t),
	
	pca_initelems_int8,
	pca_copyelems_int8,
	pca_getzval_int8,
	pca_setzval_int8
};
static carray_obj_fntab uint32_fntab = {
	CAT_UINT32,
	sizeof(uint32_t),

	pca_initelems_uint32,
	pca_copyelems_uint32,
	pca_getzval_uint32,
	pca_setzval_uint32
};


void carray_etype_ctor(p_carray_obj this, int etype) {
	switch(etype) {
		case CAT_INT8:
			this->fntab = &int8_fntab;
			break;
		case CAT_UINT32: {
			this->fntab = &uint32_fntab;
			break;
		}
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


