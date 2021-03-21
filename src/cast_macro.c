
#ifndef PHIZ_CAST_TYPE
#error PHIZ_CAST_TYPE not defined
#endif

#ifndef PHIZ_ZVAL_TYPE
#error PHIZ_ZVAL_TYPE not defined
#endif


#ifndef PHIZ_CAST_MIN
#error PHIZ_CAST_MIN not defined
#endif


#ifndef PHIZ_CAST_MAX
#error PHIZ_CAST_MAX not defined
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

static void pca_getzval_name
( p_carray_obj this, zend_long offset,  zval* retval)
{
	t_ctype value = *((t_ctype*) this->elements + offset);
	ret_phiz_zval(retval,value);
}

#define pca_setzval_name  CAT2(pca_setzval_,PHIZ_CAST_NAME)

static int pca_setzval_name
( p_carray_obj this, zend_long offset,  zval* setval)
{
	if (Z_TYPE_P(setval) != CAT2(IS_,PHIZ_ZVAL_TYPE)){
		return CATE_TYPE;
	}
	t_ctype val = PHIZ_ZVAL_CAST_TYPE(setval);

	if (val < PHIZ_CAST_MIN || val > PHIZ_CAST_MAX) {
		return  CATE_RANGE;
	}
	*( (t_ctype*) this->elements + offset) = val;
	return CATE_OK;
}

static carray_obj_fntab CAT2(PHIZ_CAST_NAME, _fntab) = {
	PHIZ_CARRAY_ETYPE,
	sizeof(PHIZ_CAST_TYPE),
	pca_type_name,
	pca_init_name,
	pca_copy_name,
	pca_getzval_name,
	pca_setzval_name
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
#undef PHIZ_CAST_MIN
#undef PHIZ_CAST_MAX