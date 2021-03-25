/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 753f706b8a6b3e8d51eb60c17afd8863a4d65385 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str8___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str8___toString, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_Str8_getIterator, 0, 0, Iterator, 0)
ZEND_END_ARG_INFO()


ZEND_METHOD(Str8, __construct);
ZEND_METHOD(Str8, __toString);
ZEND_METHOD(Str8, getIterator);


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
