#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#include "phiz_exceptions.h"
#include "phiz_carray.h"


PHPAPI zend_class_entry *phiz_ce_RuntimeException;

#define phiz_ce_Exception zend_ce_exception


/* {{{ spl_register_sub_class */
PHPAPI void phiz_register_sub_class(zend_class_entry** ppce, 
	zend_class_entry* parent_ce, char* class_name, void* obj_ctor,
	const zend_function_entry* function_list)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY_EX(ce, class_name, strlen(class_name), function_list);
	*ppce = zend_register_internal_class_ex(&ce, parent_ce);

	/* entries changed by initialize */
	if (obj_ctor) {
		(*ppce)->create_object = obj_ctor;
	} else {
		(*ppce)->create_object = parent_ce->create_object;
	}
}

/* {{{ PHP_MINIT_FUNCTION(spl_exceptions) */
PHP_MINIT_FUNCTION(phiz_exceptions)
{

    REGISTER_PHIZ_SUB_CLASS_EX(RuntimeException, Exception, NULL, NULL);

	return SUCCESS;
}
/* }}} */