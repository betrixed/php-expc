/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 20207cb04c5c30ec59799412f8689ae0459080fe */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_toml_parse, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Ctoml_parse, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Ctoml_matchBase, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Ctoml_matchTime, 0, 1, DayTime, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Ctoml_matchInt arginfo_class_Ctoml_matchBase

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Ctoml_matchDateTime, 0, 1, DateTime, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Ctoml_matchBool, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Ctoml_matchFloatDot, 0, 1, IS_DOUBLE, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Ctoml_matchFloatExp arginfo_class_Ctoml_matchFloatDot


ZEND_FUNCTION(toml_parse);
ZEND_METHOD(Ctoml, parse);
ZEND_METHOD(Ctoml, matchBase);
ZEND_METHOD(Ctoml, matchTime);
ZEND_METHOD(Ctoml, matchInt);
ZEND_METHOD(Ctoml, matchDateTime);
ZEND_METHOD(Ctoml, matchBool);
ZEND_METHOD(Ctoml, matchFloatDot);
ZEND_METHOD(Ctoml, matchFloatExp);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(toml_parse, arginfo_toml_parse)
	ZEND_FE_END
};


static const zend_function_entry class_Ctoml_methods[] = {
	ZEND_ME(Ctoml, parse, arginfo_class_Ctoml_parse, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchBase, arginfo_class_Ctoml_matchBase, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchTime, arginfo_class_Ctoml_matchTime, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchInt, arginfo_class_Ctoml_matchInt, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchDateTime, arginfo_class_Ctoml_matchDateTime, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchBool, arginfo_class_Ctoml_matchBool, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchFloatDot, arginfo_class_Ctoml_matchFloatDot, ZEND_ACC_PUBLIC)
	ZEND_ME(Ctoml, matchFloatExp, arginfo_class_Ctoml_matchFloatExp, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
