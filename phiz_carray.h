#ifndef PHIZ_CARRAY_H
#define PHIZ_CARRAY_H

#include <stdint.h>

extern PHPAPI zend_class_entry *phiz_ce_CArray;
extern PHPAPI zend_class_entry *phiz_ce_RuntimeException;

PHP_MINIT_FUNCTION(phiz_carray);


ZEND_FUNCTION(str_camel);
ZEND_FUNCTION(str_uncamel);




PHPAPI zend_long spl_offset_convert_to_long(zval *offset);

#endif	