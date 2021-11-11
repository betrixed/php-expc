#ifndef PHIZ_CREGX_H
#define PHIZ_CREGX_H

#include <stdint.h>

#ifdef HAVE_BUNDLED_PCRE
#include "pcre2lib/pcre2.h"
#else
#include "pcre2.h"
#endif


extern PHPAPI zend_class_entry *phiz_ce_Cregx;

PHP_MINIT_FUNCTION(phiz_cregx);

typedef struct _pcre_re {
	pcre2_code *re;
	uint32_t preg_options;
	uint32_t capture_count;
	uint32_t name_count;
	uint32_t compile_options;
} pcre_re;

typedef struct _phiz_cregx* pz_cregx;

typedef struct _phiz_cregx {
	zval                 expr;
	zval                 options;
	pcre_re     		*code;
	zend_object          std;
} phiz_cregx;

static inline  pz_cregx phiz_cregx_from_obj(zend_object *obj)
{
	return (pz_cregx)((char*)(obj) - XtOffsetOf(phiz_cregx, std));
}

static inline  pz_cregx Z_PHIZ_CREGX_P(zval *zv)
{
	return (pz_cregx)((char*) Z_OBJ_P((zv)) - XtOffsetOf(phiz_cregx, std));
}

#endif	