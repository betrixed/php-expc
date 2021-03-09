/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4cddab79a2195c8f5b93082af8d27addd61befbc */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str8___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, s, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Str8_current, 0, 0, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Str8_key arginfo_class_Str8_current

#define arginfo_class_Str8_bytes arginfo_class_Str8_current

#define arginfo_class_Str8_byteslen arginfo_class_Str8_current

#define arginfo_class_Str8_next arginfo_class_Str8_current

#define arginfo_class_Str8_rewind arginfo_class_Str8_current

#define arginfo_class_Str8_valid arginfo_class_Str8_current

#define arginfo_class_Str8_iteratee arginfo_class_Str8_current


ZEND_METHOD(Str8, __construct);
ZEND_METHOD(Str8, current);
ZEND_METHOD(Str8, key);
ZEND_METHOD(Str8, bytes);
ZEND_METHOD(Str8, byteslen);
ZEND_METHOD(Str8, next);
ZEND_METHOD(Str8, rewind);
ZEND_METHOD(Str8, valid);
ZEND_METHOD(Str8, iteratee);


static const zend_function_entry class_Str8_methods[] = {
	ZEND_ME(Str8, __construct, arginfo_class_Str8___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, current, arginfo_class_Str8_current, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, key, arginfo_class_Str8_key, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, bytes, arginfo_class_Str8_bytes, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, byteslen, arginfo_class_Str8_byteslen, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, next, arginfo_class_Str8_next, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, rewind, arginfo_class_Str8_rewind, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, valid, arginfo_class_Str8_valid, ZEND_ACC_PUBLIC)
	ZEND_ME(Str8, iteratee, arginfo_class_Str8_iteratee, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_Str8(zend_class_entry *class_entry_Iterator)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "Str8", class_Str8_methods);
	class_entry = zend_register_internal_class_ex(&ce, NULL);
	zend_class_implements(class_entry, 1, class_entry_Iterator);

	return class_entry;
}
