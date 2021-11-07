#ifndef CARRAY_OBJ_H
#define CARRAY_OBJ_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "php.h"

typedef enum _CArray_EType {
 	CAT_UNSIGNED = 1,
	CAT_INT8 = 2,
	CAT_UINT8 = 3,
	CAT_INT16 = 4,
	CAT_UINT16 = 5,
	CAT_INT32 = 8,
	CAT_UINT32 = 9,
	CAT_INT64 = 16,
	CAT_REAL32 = 32,
	CAT_REAL64 = 64,
	CAT_MIXED = 128
} CArray_EType;

typedef enum _CArray_Error {
	CATE_OK = 0,
	CATE_TYPE = -1,
	CATE_RANGE = -2,
	CATE_OFFSET = -3
} CArray_Error;

typedef struct _carray_obj* p_carray_obj;

typedef struct _garray*		p_garray;

typedef struct _gen_array_fntab {
	uint64_t  	esize;		// element storage in bytes
	char*		ename;	// common type name
	uint32_t	ctdt;   // 0|1 Must call elements ctor and dtor  
	uint32_t	etype;  // type indicator    ma
	
	void (*init_elems)(p_garray self, long from, long to);
	void (*dtor_elems)(p_garray self, long from, long to);
	void (*copy_elems)(p_garray self, long offset,  
		               p_garray other, long begin, long end);

}
gen_array_fntab;

typedef struct _zarray_fntab {
	zval* (*get_zval)(p_carray_obj self, zend_long offset, zval *zp); 
	int (*set_zval)(p_carray_obj self, zend_long offset, zval *zp); 
	zend_object_iterator_funcs		zit_funcs;
}
zarray_fntab;

typedef struct _array_head {
	void* 				 	elements;	
	size_t				 	size;
	size_t					capacity;
}
array_head;

typedef struct _garray {
	array_head			head;
	gen_array_fntab*    fntab;
}
garray;

typedef struct _carray_obj {
	garray              gen;
	zarray_fntab*       zntab;
}
carray_obj;

typedef struct _phiz_carray_obj* pz_carray;

typedef struct _phiz_carray_obj {
	carray_obj           cobj;		// embedded polymorphic array
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

typedef struct _carray_quickit {
	zend_object_iterator intern;
	zval                 result; // somehow to pass back the current value
	zend_long            current;
} carray_quickit;

static inline  pz_carray phiz_carray_from_obj(zend_object *obj)
{
	return (pz_carray)((char*)(obj) - XtOffsetOf(phiz_carray_obj, std));
}

static inline  pz_carray Z_PHIZ_CARRAY_P(zval *zv)
{
	return (pz_carray)((char*) Z_OBJ_P((zv)) - XtOffsetOf(phiz_carray_obj, std));
}
// forward declarations
void carray_etype_ctor(p_carray_obj this, int etype);
void carray_copy_ctor(p_carray_obj this, p_carray_obj from);

// works on all carray_obj
int  pca_pushback(carray_obj*, zval* value);
void gen_resize(garray* this, long size);
void pca_reserve(garray* this, long size);
void pca_alloc(garray* this, long size);
void pca_free(garray* this);
char* pca_get_type_str(int etype);

#endif