/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 72a4b4fc0d160a615b4acd05e9adfd8402aa7e0a */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_route_extract_params, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_camel, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seperate, IS_STRING, 0, "\"_\"")
ZEND_END_ARG_INFO()

#define arginfo_str_uncamel arginfo_str_camel


ZEND_FUNCTION(route_extract_params);
ZEND_FUNCTION(str_camel);
ZEND_FUNCTION(str_uncamel);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(route_extract_params, arginfo_route_extract_params)
	ZEND_FE(str_camel, arginfo_str_camel)
	ZEND_FE(str_uncamel, arginfo_str_uncamel)
	ZEND_FE_END
};
