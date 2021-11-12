/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: bd0aea0a13f270b5adf4b8ba8e61944d0a5d1faf */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Cregx___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, exp, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, global, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Cregx___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Cregx_match, 0, 2, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Cregx_captures, 0, 0, IS_ARRAY, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Cregx_clear, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Cregx, __construct);
ZEND_METHOD(Cregx, __toString);
ZEND_METHOD(Cregx, match);
ZEND_METHOD(Cregx, captures);
ZEND_METHOD(Cregx, clear);


static const zend_function_entry class_Cregx_methods[] = {
	ZEND_ME(Cregx, __construct, arginfo_class_Cregx___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, __toString, arginfo_class_Cregx___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, match, arginfo_class_Cregx_match, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, captures, arginfo_class_Cregx_captures, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, clear, arginfo_class_Cregx_clear, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
