/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c23bb5d2e6ba83c910a275690d9ac6b65498f482 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, cptype, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray_count, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CArray_toArray arginfo_class_CArray_count

#define arginfo_class_CArray_getSize arginfo_class_CArray_count

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray_setSize, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray_offsetExists, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO()

#define arginfo_class_CArray_offsetGet arginfo_class_CArray_offsetExists

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_CArray_offsetSet, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_CArray_offsetUnset arginfo_class_CArray_offsetExists


ZEND_METHOD(CArray, __construct);
ZEND_METHOD(CArray, count);
ZEND_METHOD(CArray, toArray);
ZEND_METHOD(CArray, getSize);
ZEND_METHOD(CArray, setSize);
ZEND_METHOD(CArray, offsetExists);
ZEND_METHOD(CArray, offsetGet);
ZEND_METHOD(CArray, offsetSet);
ZEND_METHOD(CArray, offsetUnset);


static const zend_function_entry class_CArray_methods[] = {
	ZEND_ME(CArray, __construct, arginfo_class_CArray___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, count, arginfo_class_CArray_count, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, toArray, arginfo_class_CArray_toArray, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, getSize, arginfo_class_CArray_getSize, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, setSize, arginfo_class_CArray_setSize, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, offsetExists, arginfo_class_CArray_offsetExists, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, offsetGet, arginfo_class_CArray_offsetGet, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, offsetSet, arginfo_class_CArray_offsetSet, ZEND_ACC_PUBLIC)
	ZEND_ME(CArray, offsetUnset, arginfo_class_CArray_offsetUnset, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
