/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 35b9aaa6108d5ac1933f86b849b2950caa809257 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_route_extract_params, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_camel, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, seperate, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_str_uncamel arginfo_str_camel

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_utf8c, 0, 3, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, offset, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, code, IS_LONG, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(route_extract_params);
ZEND_FUNCTION(str_camel);
ZEND_FUNCTION(str_uncamel);
ZEND_FUNCTION(str_utf8c);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(route_extract_params, arginfo_route_extract_params)
	ZEND_FE(str_camel, arginfo_str_camel)
	ZEND_FE(str_uncamel, arginfo_str_uncamel)
	ZEND_FE(str_utf8c, arginfo_str_utf8c)
	ZEND_FE_END
};
