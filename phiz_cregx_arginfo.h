/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d10f0f65fbc084f71f42c750375815f4ea1e2498 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Cregx___construct, 0, 0, 2)
	ZEND_ARG_TYPE_INFO(0, exp, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Cregx___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Cregx_match, 0, 2, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Cregx, __construct);
ZEND_METHOD(Cregx, __toString);
ZEND_METHOD(Cregx, match);


static const zend_function_entry class_Cregx_methods[] = {
	ZEND_ME(Cregx, __construct, arginfo_class_Cregx___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, __toString, arginfo_class_Cregx___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, match, arginfo_class_Cregx_match, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
