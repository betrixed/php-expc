/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 04493da0957c8db2ba6f647930d0ce515b7cb5d9 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_DayTime___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, t24, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DayTime_set, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, hours, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, mins, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, seconds, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DayTime_split, 0, 3, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(1, h24, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, min, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(1, sec, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DayTime_format, 0, 0, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, flags, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_DayTime___toString, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(DayTime, __construct);
ZEND_METHOD(DayTime, set);
ZEND_METHOD(DayTime, split);
ZEND_METHOD(DayTime, format);
ZEND_METHOD(DayTime, __toString);


static const zend_function_entry class_DayTime_methods[] = {
	ZEND_ME(DayTime, __construct, arginfo_class_DayTime___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(DayTime, set, arginfo_class_DayTime_set, ZEND_ACC_PUBLIC)
	ZEND_ME(DayTime, split, arginfo_class_DayTime_split, ZEND_ACC_PUBLIC)
	ZEND_ME(DayTime, format, arginfo_class_DayTime_format, ZEND_ACC_PUBLIC)
	ZEND_ME(DayTime, __toString, arginfo_class_DayTime___toString, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};
