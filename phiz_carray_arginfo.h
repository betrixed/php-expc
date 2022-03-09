/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6b3fd28b8e06bb4e984a6a0893ecb4e4b2ce8a48 */

#ifndef IS_MIXED
#define IS_MIXED 0
#endif

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Carray___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, typeEnum, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Carray_count, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Carray_toArray, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Carray_getTypeEnum arginfo_class_Carray_toArray

#define arginfo_class_Carray_getTypeName arginfo_class_Carray_toArray

#define arginfo_class_Carray_getTypeSize arginfo_class_Carray_toArray

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Carray_reserve, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Carray_size arginfo_class_Carray_toArray

#define arginfo_class_Carray_capacity arginfo_class_Carray_toArray

#define arginfo_class_Carray_resize arginfo_class_Carray_reserve

#define arginfo_class_Carray_signedType arginfo_class_Carray_toArray

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Carray_offsetExists, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Carray_offsetGet, 0, 1, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Carray_offsetSet, 0, 2, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_MIXED, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Carray_offsetUnset, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, index, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Carray_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Carray_sub, 0, 2, CArray, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, len, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Csu8___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Csu8___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Csu8_getIterator arginfo_class_Carray_getIterator

#define arginfo_class_Csu32___construct arginfo_class_Csu8___construct

#define arginfo_class_Csu32___toString arginfo_class_Csu8___toString


ZEND_METHOD(Carray, __construct);
ZEND_METHOD(Carray, count);
ZEND_METHOD(Carray, toArray);
ZEND_METHOD(Carray, getTypeEnum);
ZEND_METHOD(Carray, getTypeName);
ZEND_METHOD(Carray, getTypeSize);
ZEND_METHOD(Carray, reserve);
ZEND_METHOD(Carray, size);
ZEND_METHOD(Carray, capacity);
ZEND_METHOD(Carray, resize);
ZEND_METHOD(Carray, signedType);
ZEND_METHOD(Carray, offsetExists);
ZEND_METHOD(Carray, offsetGet);
ZEND_METHOD(Carray, offsetSet);
ZEND_METHOD(Carray, offsetUnset);
ZEND_METHOD(Carray, getIterator);
ZEND_METHOD(Carray, sub);
ZEND_METHOD(Csu8, __construct);
ZEND_METHOD(Csu8, __toString);
ZEND_METHOD(Csu8, getIterator);
ZEND_METHOD(Csu32, __construct);
ZEND_METHOD(Csu32, __toString);


static const zend_function_entry class_Carray_methods[] = {
	ZEND_ME(Carray, __construct, arginfo_class_Carray___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, count, arginfo_class_Carray_count, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, toArray, arginfo_class_Carray_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, getTypeEnum, arginfo_class_Carray_getTypeEnum, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, getTypeName, arginfo_class_Carray_getTypeName, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, getTypeSize, arginfo_class_Carray_getTypeSize, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, reserve, arginfo_class_Carray_reserve, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, size, arginfo_class_Carray_size, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, capacity, arginfo_class_Carray_capacity, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, resize, arginfo_class_Carray_resize, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, signedType, arginfo_class_Carray_signedType, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, offsetExists, arginfo_class_Carray_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, offsetGet, arginfo_class_Carray_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, offsetSet, arginfo_class_Carray_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, offsetUnset, arginfo_class_Carray_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, getIterator, arginfo_class_Carray_getIterator, ZEND_ACC_PUBLIC)
	ZEND_ME(Carray, sub, arginfo_class_Carray_sub, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Csu8_methods[] = {
	ZEND_ME(Csu8, __construct, arginfo_class_Csu8___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Csu8, __toString, arginfo_class_Csu8___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Csu8, getIterator, arginfo_class_Csu8_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Csu32_methods[] = {
	ZEND_ME(Csu32, __construct, arginfo_class_Csu32___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Csu32, __toString, arginfo_class_Csu32___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
