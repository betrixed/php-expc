
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "carray_obj.h"

void pca_alloc(garray* this, long size) {
	if (size > 0) {
		this->head.size = 0; /* reset size in case ecalloc() fails */
		this->head.elements = safe_emalloc(size, this->fntab->esize, 0);
		this->head.size = size;
	}
}

bool pca_is_empty(garray* this) {
	if (this->head.elements) {
		ZEND_ASSERT(this->head.size > 0);
		return false;
	}
	ZEND_ASSERT(this->head.size == 0);
	return true;
}

void pca_free(garray* this)
{
	if (!pca_is_empty(this)) {
		efree(this->head.elements);
		this->head.elements = NULL;
		this->head.capacity = 0;
		this->head.size = 0;
	}
}

// pre-calculated.  *2 up to 512, *1.61803 after,
// up to  165175  . After that, *1.5

long next_cap_mark(long size) {

#define  kmax_cap_index 20
static long capacity_mark[kmax_cap_index] = 
{
	4, 8, 16, 32, 64, 128, 256, 512, 829, 1342, // 10
	2172, 3515, 5688, 9204, 14893, 24098, 38992, 63091, 102084, 165175, // 10
};
	int mid;
	int lo = 0;
	int hi = kmax_cap_index; // maximum index
	long value;


	while (hi > lo) {
		mid = (lo + hi)/2; // maximum index
		value = capacity_mark[mid];
		if (value < size) {
			// index too low!
			lo = mid + 1;
		}
		else if (value == size) {
			return value;
		}
		else { // value > size
			hi = mid;
		}
	}
	if (lo < kmax_cap_index) {
		value = capacity_mark[lo];
	}
	else {
		// Go for bust, assume grow by push
		value = (long) floor(1.5*(size-1));
	}
	
	//printf("size %d, value %d lo %d hi %d", size, value, lo, hi);

	return value; // a should be next larger
}

// Make allocations and deallocations go here
// Can set exact capacity, not below current size
void pca_reserve(garray* this, long newCapacity) {
	int esize;

	if (newCapacity < this->head.size) {
		newCapacity = this->head.size;
	}
	if (newCapacity == 0 && this->head.capacity > 0) {
		if (this->head.elements) {
			efree(this->head.elements);
			this->head.elements = NULL;
			this->head.capacity = 0;
			return;
		}
	}
	esize = this->fntab->esize;
	if (this->head.capacity == 0) {
		this->head.elements = safe_emalloc(newCapacity, esize, 0);
	}
	else {
		this->head.elements = safe_erealloc(this->head.elements, newCapacity, esize, 0);
	}
	this->head.capacity = newCapacity;
}

int pca_pushback(carray_obj* this, zval* value) {
	// initialize with value
	long offset = this->gen.head.size;
	gen_resize(&this->gen,offset+1);
	return this->zntab->set_zval(this,offset,value);
}

void gen_resize(garray* this, long size) {
	gen_array_fntab* fntab;

	if (this->head.size == size) {
		return;
	}
	fntab = this->fntab;
	if (size > this->head.size) {
		if (size > this->head.capacity) {
			pca_reserve(this, next_cap_mark(size));
		}
		if (fntab->ctdt) {
			fntab->init_elems(this,this->head.size,size);
		}
		this->head.size = size;
		return;
	}
	if (size == 0) {
		if (this->head.size > 0 && fntab->ctdt) {
			fntab->dtor_elems(this,0,this->head.size);
			this->head.size = 0;
		}
		pca_reserve(this,0);
		return;
	}
	// 0 < size < this->size
	if (fntab->ctdt) {
		fntab->dtor_elems(this,size,this->head.size);
	}	
	this->head.size = size;
	return;
}


static void carray_quickit_dtor(zend_object_iterator *iter)
{
	zval_ptr_dtor(&iter->data);
	zval_ptr_dtor( &((carray_quickit*)iter)->result );
}

static void carray_quickit_rewind(zend_object_iterator *iter)
{
	((carray_quickit*)iter)->current = 0;
}

static int carray_quickit_valid(zend_object_iterator *iter)
{
	carray_quickit     *iterator = (carray_quickit*)iter;
	pz_carray object   = Z_PHIZ_CARRAY_P(&iter->data);

	if (iterator->current >= 0 && iterator->current < object->cobj.gen.head.size) {
		return SUCCESS;
	}

	return FAILURE;
}

static zval *carray_quickit_get_current_data(zend_object_iterator *iter)
{
	carray_quickit     *iterator = (carray_quickit*)iter;
	pz_carray object   = Z_PHIZ_CARRAY_P(&iter->data);

	//zindex = iterator->current;
	/* if (zindex < 0 || zindex >= object->cobj.size) {
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		return NULL;
	}
	*/
	p_carray_obj pobj = &object->cobj;

	return pobj->zntab->get_zval(pobj,iterator->current,&iterator->result);

}

static void carray_quickit_get_current_key(zend_object_iterator *iter, zval *key)
{
	ZVAL_LONG(key, ((carray_quickit*)iter)->current);
}

static void carray_quickit_move_forward(zend_object_iterator *iter)
{
	((carray_quickit*)iter)->current++;
}


// CAT_INT8
#define PHIZ_CARRAY_ETYPE CAT_INT8
#define PHIZ_CAST_TYPE int8_t
#define PHIZ_CAST_NAME int8
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_ZVAL_CAST_TEMP long
#define PHIZ_CAST_MIN CHAR_MIN
#define PHIZ_CAST_MAX CHAR_MAX
#include "cast_macro.c"

// CAT_UINT8
#define PHIZ_CARRAY_ETYPE CAT_UINT8
#define PHIZ_CAST_TYPE uint8_t
#define PHIZ_CAST_NAME uint8
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_ZVAL_CAST_TEMP long
#define PHIZ_CAST_MIN 0
#define PHIZ_CAST_MAX UINT8_MAX
#include "cast_macro.c"

// CAT_INT16
#define PHIZ_CARRAY_ETYPE CAT_INT16
#define PHIZ_CAST_TYPE  int16_t
#define PHIZ_CAST_NAME  int16
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_ZVAL_CAST_TEMP long
#define PHIZ_CAST_MIN INT16_MIN
#define PHIZ_CAST_MAX INT16_MAX
#include "cast_macro.c"


// CAT_UINT16
#define PHIZ_CARRAY_ETYPE CAT_UINT16
#define PHIZ_CAST_TYPE  uint16_t
#define PHIZ_CAST_NAME  uint16
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_ZVAL_CAST_TEMP long
#define PHIZ_CAST_MIN 0
#define PHIZ_CAST_MAX UINT16_MAX
#include "cast_macro.c"



// CAT_INT32
#define PHIZ_CARRAY_ETYPE CAT_INT32
#define PHIZ_CAST_TYPE int32_t
#define PHIZ_CAST_NAME int32
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_ZVAL_CAST_TEMP long
#define PHIZ_CAST_MIN INT32_MIN
#define PHIZ_CAST_MAX INT32_MAX
#include "cast_macro.c"

// CAT_UINT32
#define PHIZ_CARRAY_ETYPE CAT_UINT32
#define PHIZ_CAST_TYPE uint32_t
#define PHIZ_CAST_NAME uint32
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#define PHIZ_ZVAL_CAST_TEMP long
#define PHIZ_CAST_MIN 0
#define PHIZ_CAST_MAX UINT32_MAX
#include "cast_macro.c"

// CAT_INT64
#define PHIZ_CARRAY_ETYPE CAT_INT64
#define PHIZ_CAST_TYPE int64_t
#define PHIZ_CAST_NAME int64
#define PHIZ_ZVAL_TYPE LONG
#define PHIZ_ZVAL_CAST_TYPE Z_LVAL_P
#include "cast_macro.c"


// CAT_REAL32
#define PHIZ_CARRAY_ETYPE CAT_REAL32
#define PHIZ_CAST_TYPE float
#define PHIZ_CAST_NAME float
#define PHIZ_ZVAL_TYPE DOUBLE
#define PHIZ_ZVAL_CAST_TYPE Z_DVAL_P
#define PHIZ_ZVAL_CAST_TEMP double
#define PHIZ_CAST_MIN -FLT_MAX
#define PHIZ_CAST_MAX FLT_MAX
#include "cast_macro.c"

// CAT_REAL64

#define PHIZ_CARRAY_ETYPE CAT_REAL64
#define PHIZ_CAST_TYPE double
#define PHIZ_CAST_NAME double
#define PHIZ_ZVAL_TYPE DOUBLE
#define PHIZ_ZVAL_CAST_TYPE Z_DVAL_P
#include "cast_macro.c"

// The zval array functions are special
static void pca_init_zval (garray* this, long from, long to) 
{
	zval* begin = (zval*) this->head.elements + from;
	zval* end = (zval*) this->head.elements + to;
	while (begin != end) {
		 ZVAL_NULL(begin);
		 begin++;
	}
}
static void pca_dtor_zval (garray* this, long from, long to) 
{
	zval* begin = (zval*) this->head.elements + from;
	zval* end = (zval*) this->head.elements + to;
	while (begin != end) {
		 zval_dtor(begin);
		 ZVAL_NULL(begin);
		 begin++;
	}
}

static void pca_copy_zval
 (garray* this, zend_long offset, garray* other, zend_long begin, zend_long end)
{
	zval *to = (zval*) this->head.elements + offset;
	zval *bgp = (zval*) other->head.elements + begin;
	zval *ep = (zval*) other->head.elements + end;

	while (bgp != ep) {
		ZVAL_COPY(to++,bgp++);
	}
}

static zval* pca_get_zval
( p_carray_obj this, zend_long offset,  zval* retval)
{
	return (zval*) this->gen.head.elements + offset;
}

#define pca_setzval_name   CAT2(pca_setzval_,PHIZ_CAST_NAME)

static int pca_set_zval
( p_carray_obj this, zend_long offset,  zval* setval)
{
	zval* val = (zval*) this->gen.head.elements + offset;
	zval_ptr_dtor(val);
	ZVAL_COPY_DEREF(val, setval);
	return CATE_OK;
}


static zval* pcait_getdata_zval
(zend_object_iterator *iter)
{
	carray_quickit  *iterator = (carray_quickit*)iter;
	pz_carray object   = Z_PHIZ_CARRAY_P(&iter->data);
	p_carray_obj this = &object->cobj;
	return (zval*) this->gen.head.elements + iterator->current;
}

static gen_array_fntab mixed_fntab = {
	sizeof(zval),
	"mixed",
	1,
	CAT_MIXED,
	pca_init_zval,
	pca_dtor_zval,
	pca_copy_zval
};

static zarray_fntab mixed_zntab = {
	pca_get_zval,
	pca_set_zval,
	{
		carray_quickit_dtor,
		carray_quickit_valid,
		pcait_getdata_zval,
		carray_quickit_get_current_key,
		carray_quickit_move_forward,
		carray_quickit_rewind
		/*, NULL, NULL */
	}
};

void carray_etype_ctor(p_carray_obj this, int etype) {
	switch(etype) {
		case CAT_INT8:
			this->zntab = &int8_zntab;
			this->gen.fntab = &int8_fntab;
			break;
		case CAT_UINT8:
			this->zntab = &uint8_zntab;
			this->gen.fntab = &uint8_fntab;
			break;
		case CAT_INT16:
			this->zntab = &int16_zntab;
			this->gen.fntab = &uint16_fntab;
			break;
		case CAT_UINT16:
			this->zntab = &uint16_zntab;
			this->gen.fntab = &uint16_fntab;
			break;
		case CAT_INT32: 
			this->zntab = &int32_zntab;
			this->gen.fntab = &int32_fntab;
			break;
		case CAT_UINT32: 
			this->zntab = &uint32_zntab;
			this->gen.fntab = &uint32_fntab;
			break;
		case CAT_INT64: 
			this->zntab = &int64_zntab;
			this->gen.fntab = &int64_fntab;
			break;
		/*case CAT_UINT64: 
			this->fntab = &uint64_fntab;
			break;*/
		case CAT_REAL32: 
			this->zntab = &float_zntab;
			this->gen.fntab = &float_fntab;
			break;
		case CAT_REAL64: 
			this->zntab = &double_zntab;
			this->gen.fntab = &double_fntab;
			break;
		case CAT_MIXED:
			this->zntab = &mixed_zntab;
			this->gen.fntab = &mixed_fntab;
			break;
	}
	this->gen.head.elements = NULL;
	this->gen.head.capacity = 0;
	this->gen.head.size = 0;
}

void carray_copy_ctor(p_carray_obj this, p_carray_obj from)
{
	carray_etype_ctor(this, from->gen.fntab->etype);
	int from_size = from->gen.head.size;
	if (from_size > 0) {
		pca_alloc(&this->gen,from_size);
		(this->gen.fntab)->copy_elems(&this->gen,0,&from->gen,0,from_size);
	}
}


