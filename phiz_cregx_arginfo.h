/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 70c722c68b0f0a0de04effc8b70e80665601666c */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_toml_parse, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
ZEND_END_ARG_INFO()

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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Cregx_setExpr, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, exp, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, global, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Ctoml_matchInt, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Ctoml_matchDateTime, 0, 1, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Ctoml_matchBool, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Ctoml_matchFloatDot, 0, 1, double, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Ctoml_matchFloatExp arginfo_class_Ctoml_matchFloatDot


ZEND_FUNCTION(toml_parse);
ZEND_METHOD(Cregx, __construct);
ZEND_METHOD(Cregx, __toString);
ZEND_METHOD(Cregx, match);
ZEND_METHOD(Cregx, captures);
ZEND_METHOD(Cregx, clear);
ZEND_METHOD(Cregx, setExpr);
ZEND_METHOD(Ctoml, matchInt);
ZEND_METHOD(Ctoml, matchDateTime);
ZEND_METHOD(Ctoml, matchBool);
ZEND_METHOD(Ctoml, matchFloatDot);
ZEND_METHOD(Ctoml, matchFloatExp);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(toml_parse, arginfo_toml_parse)
	ZEND_FE_END
};


static const zend_function_entry class_Cregx_methods[] = {
	ZEND_ME(Cregx, __construct, arginfo_class_Cregx___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, __toString, arginfo_class_Cregx___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, match, arginfo_class_Cregx_match, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, captures, arginfo_class_Cregx_captures, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, clear, arginfo_class_Cregx_clear, ZEND_ACC_PUBLIC)
	ZEND_ME(Cregx, setExpr, arginfo_class_Cregx_setExpr, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


static const zend_function_entry class_Ctoml_methods[] = {
	ZEND_ME(Ctoml, matchInt, arginfo_class_Ctoml_matchInt, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchDateTime, arginfo_class_Ctoml_matchDateTime, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchBool, arginfo_class_Ctoml_matchBool, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchFloatDot, arginfo_class_Ctoml_matchFloatDot, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchFloatExp, arginfo_class_Ctoml_matchFloatExp, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
