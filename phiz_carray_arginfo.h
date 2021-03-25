/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 39fb3de03bec0ef9397c4545ff5b3e88298f3d4e */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cptype, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray_count, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CArray_toArray arginfo_class_CArray_count

#define arginfo_class_CArray_getTypeEnum arginfo_class_CArray_count

#define arginfo_class_CArray_getTypeName arginfo_class_CArray_count

#define arginfo_class_CArray_getTypeSize arginfo_class_CArray_count

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray_reserve, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CArray_size arginfo_class_CArray_count

#define arginfo_class_CArray_capacity arginfo_class_CArray_count

#define arginfo_class_CArray_resize arginfo_class_CArray_reserve

#define arginfo_class_CArray_signedType arginfo_class_CArray_count

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

#define arginfo_class_CArray_offsetGet arginfo_class_CArray_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CArray_offsetUnset arginfo_class_CArray_offsetExists

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_CArray_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(CArray, __construct);
ZEND_METHOD(CArray, count);
ZEND_METHOD(CArray, toArray);
ZEND_METHOD(CArray, getTypeEnum);
ZEND_METHOD(CArray, getTypeName);
ZEND_METHOD(CArray, getTypeSize);
ZEND_METHOD(CArray, reserve);
ZEND_METHOD(CArray, size);
ZEND_METHOD(CArray, capacity);
ZEND_METHOD(CArray, resize);
ZEND_METHOD(CArray, signedType);
ZEND_METHOD(CArray, offsetExists);
ZEND_METHOD(CArray, offsetGet);
ZEND_METHOD(CArray, offsetSet);
ZEND_METHOD(CArray, offsetUnset);
ZEND_METHOD(CArray, getIterator);


static const zend_function_entry class_CArray_methods[] = {
	ZEND_ME(CArray, __construct, arginfo_class_CArray___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, count, arginfo_class_CArray_count, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, toArray, arginfo_class_CArray_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, getTypeEnum, arginfo_class_CArray_getTypeEnum, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, getTypeName, arginfo_class_CArray_getTypeName, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, getTypeSize, arginfo_class_CArray_getTypeSize, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, reserve, arginfo_class_CArray_reserve, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, size, arginfo_class_CArray_size, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, capacity, arginfo_class_CArray_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, resize, arginfo_class_CArray_resize, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, signedType, arginfo_class_CArray_signedType, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, offsetExists, arginfo_class_CArray_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, offsetGet, arginfo_class_CArray_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, offsetSet, arginfo_class_CArray_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, offsetUnset, arginfo_class_CArray_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, getIterator, arginfo_class_CArray_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
