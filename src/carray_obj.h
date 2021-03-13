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
	CAT_REAL32 = 16,
	CAT_REAL64 = 32
} CArray_EType;

typedef enum _CArray_Error {
	CATE_OK = 0,
	CATE_TYPE = -1,
	CATE_RANGE = -2,
	CATE_OFFSET = -3
} CArray_Error;

typedef struct _carray_obj* p_carray_obj;

typedef struct _carray_obj_fntab {
	int 	etype;
	size_t  esize;
	void (*init_elems)(p_carray_obj self, long from, long to);
	void (*copy_elems)(p_carray_obj self, long offset,  p_carray_obj other, long begin, long end);
	void (*get_zval)(p_carray_obj self, long offset, zval *zp); 
	int (*set_zval)(p_carray_obj self, long offset, zval *zp); 
}
carray_obj_fntab;


typedef struct _carray_obj {
	carray_obj_fntab* 	 fntab;
	void* 				 elements;	
	size_t				 size;
}
carray_obj;

// forward declarations
void carray_etype_ctor(p_carray_obj this, int etype);
void carray_copy_ctor(p_carray_obj this, p_carray_obj from);

// works on all carray_obj
void pca_resize(p_carray_obj this, long size);
void pca_alloc(p_carray_obj this, long size);
void pca_free(p_carray_obj this);

#endif