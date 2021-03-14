#ifndef PHIZ_CARRAY_H
#define PHIZ_CARRAY_H

#include <stdint.h>

extern PHPAPI zend_class_entry *phiz_ce_CArray;
extern PHPAPI zend_class_entry *phiz_ce_RuntimeException;

PHP_MINIT_FUNCTION(phiz_carray);



#define REGISTER_PHIZ_IMPLEMENTS(class_name, interface_name) \
	zend_class_implements(phiz_ce_ ## class_name, 1, phiz_ce_ ## interface_name);

#define REGISTER_PHIZ_SUB_CLASS_EX(class_name, parent_class_name, obj_ctor, funcs) \
	phiz_register_sub_class(&phiz_ce_ ## class_name, phiz_ce_ ## parent_class_name, # class_name, obj_ctor, funcs);

#define REGISTER_PHIZ_CLASS_CONST_LONG(const_name, value) \
	zend_declare_class_constant_long(phiz_ce_CArray, const_name, sizeof(const_name)-1, (zend_long)value);


#endif	