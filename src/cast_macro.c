
#ifndef PHIZ_CAST_TYPE
#error PHIZ_CAST_TYPE not defined
#endif

#ifndef PHIZ_ZVAL_TYPE
#error PHIZ_ZVAL_TYPE not defined
#endif


#define CAT(a,b) a##b
#define CAT2(a,b) CAT(a,b)
#define CAT_STR(x) #x
#define CAT2_STR(x) CAT_STR(x)

#define t_ctype PHIZ_CAST_TYPE
#define pca_type_name CAT2_STR(PHIZ_CAST_NAME)

#define pca_init_name CAT2(pca_initelems_, PHIZ_CAST_NAME)

static void pca_init_name (p_carray_obj this, long from, long to) 
{
	t_ctype* begin = (t_ctype*) this->elements + from;
	t_ctype* end = (t_ctype*) this->elements + to;
	while (begin != end) {
		*begin++ = 0;
	}
}

#ifndef PHIX_CAST_DTOR
#define pca_dtor_name NULL
#endif


#define pca_copy_name CAT2(pca_copyelems_,PHIZ_CAST_NAME)

static void pca_copy_name
 (p_carray_obj this, zend_long offset, p_carray_obj other, zend_long begin, zend_long end)
{
	t_ctype *to = (t_ctype*) this->elements + offset;
	t_ctype *bgp = (t_ctype*) other->elements + begin;
	t_ctype *ep = (t_ctype*) other->elements + end;

	while (bgp != ep) {
		*to++ = *bgp++;
	}
}

#define pca_getzval_name CAT2(pca_getzval_,PHIZ_CAST_NAME)
#define ret_phiz_zval	CAT2(ZVAL_,PHIZ_ZVAL_TYPE)

static zval* pca_getzval_name
( p_carray_obj this, zend_long offset,  zval* retval)
{
	ret_phiz_zval(retval,*((t_ctype*) this->elements + offset));
	return retval;
}

#define pca_setzval_name   CAT2(pca_setzval_,PHIZ_CAST_NAME)

static int pca_setzval_name
( p_carray_obj this, zend_long offset,  zval* setval)
{
	if (Z_TYPE_P(setval) != CAT2(IS_,PHIZ_ZVAL_TYPE)){
		return CATE_TYPE;
	}
	#ifdef PHIZ_ZVAL_CAST_TEMP
		PHIZ_ZVAL_CAST_TEMP val = PHIZ_ZVAL_CAST_TYPE(setval);

		if (val < (PHIZ_ZVAL_CAST_TEMP)PHIZ_CAST_MIN || val > (PHIZ_ZVAL_CAST_TEMP)PHIZ_CAST_MAX) {
			return  CATE_RANGE;
		}
		*( (t_ctype*) this->elements + offset) = val;
	#else
		*( (t_ctype*) this->elements + offset) = PHIZ_ZVAL_CAST_TYPE(setval);
	#endif
	return CATE_OK;
}

#define pcait_getdata_name CAT2(pcait_getdata_,PHIZ_CAST_NAME)

static zval* pcait_getdata_name
(zend_object_iterator *iter)
{
	zval*  data;
	zval*  result;

	carray_quickit  *iterator = (carray_quickit*)iter;
	pz_carray object   = Z_PHIZ_CARRAY_P(&iter->data);

	//zindex = iterator->current;
	/* if (zindex < 0 || zindex >= object->cobj.size) {
		zend_throw_exception(phiz_ce_RuntimeException, "Index invalid or out of range", 0);
		return NULL;
	}
	*/

	result = &iterator->result;
	p_carray_obj this = &object->cobj;

	ret_phiz_zval(result,*((t_ctype*) this->elements + iterator->current));
	return result;
}

static carray_obj_fntab CAT2(PHIZ_CAST_NAME, _fntab) = {
	PHIZ_CARRAY_ETYPE,
	0,
	sizeof(PHIZ_CAST_TYPE),
	pca_type_name,
	pca_init_name,
	pca_dtor_name,
	pca_copy_name,
	pca_getzval_name,
	pca_setzval_name,
	{
		carray_quickit_dtor,
		carray_quickit_valid,
		pcait_getdata_name,
		carray_quickit_get_current_key,
		carray_quickit_move_forward,
		carray_quickit_rewind
		/*, NULL, NULL */
	}
};

#undef pca_init_name
#undef pca_copy_name
#undef pca_getzval_name
#undef pca_setzval_name

#undef PHIZ_CARRAY_ETYPE
#undef PHIZ_CAST_TYPE
#undef t_ctype
#undef PHIZ_CAST_NAME
#undef PHIZ_ZVAL_TYPE
#undef PHIZ_ZVAL_CAST_TYPE
#undef PHIZ_ZVAL_CAST_TEMP
#undef PHIZ_CAST_MIN
#undef PHIZ_CAST_MAX