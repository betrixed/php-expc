#ifndef PHIZ_CARRAY_H
#define PHIZ_CARRAY_H

#include <stdint.h>

extern PHPAPI zend_class_entry *phiz_ce_CArray;

PHP_MINIT_FUNCTION(phiz_carray);




#define REGISTER_PHIZ_IMPLEMENTS(class_name, interface_name) \
	zend_class_implements(phiz_ce_ ## class_name, 1, phiz_ce_ ## interface_name);


#define phiz_ce_Countable     zend_ce_countable
#define phiz_ce_ArrayAccess	  zend_ce_arrayaccess

#endif	