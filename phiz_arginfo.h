/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: ac0b86d7063c38c5d572e8c70b8dc69c23767b25 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_test1, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_test2, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, str, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_test_scale, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, x, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_route_extract_params, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(test1);
ZEND_FUNCTION(test2);
ZEND_FUNCTION(test_scale);
ZEND_FUNCTION(route_extract_params);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(test1, arginfo_test1)
	ZEND_FE(test2, arginfo_test2)
	ZEND_FE(test_scale, arginfo_test_scale)
	ZEND_FE(route_extract_params, arginfo_route_extract_params)
	ZEND_FE_END
};
