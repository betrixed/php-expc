/* phiz extension for PHP */

#ifndef PHP_PHIZ_H
# define PHP_PHIZ_H

extern zend_module_entry phiz_module_entry;
# define phpext_phiz_ptr &phiz_module_entry

# define PHP_PHIZ_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_PHIZ)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

ZEND_BEGIN_MODULE_GLOBALS(phiz)

ZEND_END_MODULE_GLOBALS(phiz)

ZEND_EXTERN_MODULE_GLOBALS(phiz)

extern PHPAPI void 
phiz_register_std_class(zend_class_entry ** ppce, 
	char * class_name, void * obj_ctor, 
	const zend_function_entry* function_list);


#define REGISTER_PHIZ_IMPLEMENTS(class_name, interface_name) \
	zend_class_implements(phiz_ce_ ## class_name, 1, phiz_ce_ ## interface_name);

#define REGISTER_PHIZ_SUB_CLASS_EX(class_name, parent_class_name, obj_ctor, funcs) \
	phiz_register_sub_class(&phiz_ce_ ## class_name, phiz_ce_ ## parent_class_name, # class_name, obj_ctor, funcs);

#define REGISTER_PHIZ_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(phiz_ce_CArray, const_name, sizeof(const_name)-1, (zend_long)value);


#endif	/* PHP_PHIZ_H */
