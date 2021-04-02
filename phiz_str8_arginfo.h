/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 60bb20cf16d5475dabbfc55c736c6425bdd89eac */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_str_camel, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seperate, IS_STRING, 0, "\"_\"")
ZEND_END_ARG_INFO()

#define arginfo_str_uncamel arginfo_str_camel

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str8___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str8___toString, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Str8_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(str_camel);
ZEND_FUNCTION(str_uncamel);
ZEND_METHOD(Str8, __construct);
ZEND_METHOD(Str8, __toString);
ZEND_METHOD(Str8, getIterator);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(str_camel, arginfo_str_camel)
	ZEND_FE(str_uncamel, arginfo_str_uncamel)
	ZEND_FE_END
};


static const zend_function_entry class_Str8_methods[] = {
	ZEND_ME(Str8, __construct, arginfo_class_Str8___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, __toString, arginfo_class_Str8___toString, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, getIterator, arginfo_class_Str8_getIterator, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Str8(zend_class_entry *class_entry_IteratorAggregate)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Str8", class_Str8_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_IteratorAggregate);

	return class_entry;
}
