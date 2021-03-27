/* phiz extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "zend_exceptions.h"
#include "phiz_arginfo.h"
#include "ext/standard/info.h"
#include "php_phiz.h"
#include <zend_smart_str.h>
#include <ctype.h>

#include "phiz_str8.h"
#include "phiz_carray.h"


ZEND_DECLARE_MODULE_GLOBALS(phiz)
static PHP_GINIT_FUNCTION(phiz);
static PHP_GSHUTDOWN_FUNCTION(phiz);

PHPAPI void 
phiz_register_std_class(zend_class_entry ** ppce, 
	char * class_name, void * obj_ctor, 
	const zend_function_entry* function_list)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY_EX(ce, class_name, strlen(class_name), function_list);
	*ppce = zend_register_internal_class(&ce);

	/* entries changed by initialize */
	if (obj_ctor) {
		(*ppce)->create_object = obj_ctor;
	}
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(phiz)
{
	//REGISTER_INI_ENTRIES();
	PHP_MINIT(phiz_str8)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(phiz_carray)(INIT_FUNC_ARGS_PASSTHRU);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(phiz)
{
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */




/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(phiz)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "phiz support", "enabled");
	php_info_print_table_end();
	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* {{{ phiz_module_entry */
zend_module_entry phiz_module_entry = {
	STANDARD_MODULE_HEADER,
	"phiz",					/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(phiz),							/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(phiz),							/* PHP_MSHUTDOWN - Module shutdown */
	NULL,			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(phiz),			/* PHP_MINFO - Module info */
	PHP_PHIZ_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */


#ifdef COMPILE_DL_PHIZ
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(phiz)
#endif



/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(phiz)
{

}
/* }}} */


/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

/* }}}*/


/* {{{  route_extract_paramsstring( string $s): array */
ZEND_FUNCTION(route_extract_params)
{
	char *pat = NULL;
	size_t pat_len = 0;
	HashTable*    matches;
	HashTable*    list2;

	smart_str	  route_str = {0};
	smart_str	  regex_str = {0};
	smart_str	  variable = {0};

	zval tmp;
	char* k_route = "([^/]*)";
	int   k_rlen = strlen(k_route);

	char ch = 0, prevCh = 0, xch = 0;
	int  bracketCount = 0,  parenCount = 0,  	foundPattern = 0;
	int  intermediate = 0,  matchCount = 0;		
	int  cursor = 0, 		cursorVar = 0;
	int  marker = 0,     	middle = 0;
	bool invalid = false;
	int  tmpCount = 0;
	int  found = 0;

	char* itemStr; int itemLen;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(pat, pat_len)
	ZEND_PARSE_PARAMETERS_END();
    
    // HashTable expect maxiumum size
	list2 = zend_new_array(2);
	ZVAL_ARR(return_value, list2); 

	matches = zend_new_array(4);

	for (cursor = 0; cursor < pat_len; cursor++)
	{
		ch = *(pat + cursor);
		if (parenCount == 0) {
			if (ch == '{') {
				if (bracketCount == 0) {
					marker = cursor+1;
					middle = 0;
					invalid = false;
				}
				bracketCount++;
			}
			else if (ch == '}') {
				bracketCount--;
				if (middle > 0) {
					if (bracketCount==0) {
						matchCount++;
						itemStr = (pat + marker);
						itemLen = cursor - marker;
						for(cursorVar = 0; cursorVar < itemLen; cursorVar++) {
							xch = *(itemStr + cursorVar);
							if (xch==0) {
								break;
							}
							if ((cursorVar == 0) && !isalpha(xch)) {
								invalid = true;
								break;
							}
							if (isalnum(xch) || xch=='-' || xch=='_' || xch==':') 
							{
								if (xch == ':') {
									// destroy any previous and init
									smart_str_setl(&variable, itemStr, cursorVar);
									smart_str_setl(&regex_str, itemStr+cursorVar, itemLen-cursorVar);
								}
							}
							else {
								invalid = true;
								break;
							}
						}
						if (!invalid) {
							tmpCount = matchCount;
							if ( (variable.s != NULL) && (ZSTR_LEN(variable.s) > 0) 
								&& (regex_str.s != NULL) && (ZSTR_LEN(regex_str.s) > 0) )
							{
								found = 0;
								
								itemStr = ZSTR_VAL(regex_str.s);
								itemLen = ZSTR_LEN(regex_str.s);

								for(int ix = 0; ix < itemLen; ix++)
								{
									xch = *(itemStr+ix);
									if (xch==0) {
										break;
									}
									if (!found) {
										if (xch == '(') {
											found = 1;
										}
									} 
									else {
										if (xch == ')') {
											found = 2;
											break;
										}
									}
								}
								if (found != 2) {
									smart_str_appendc(&route_str, '(');
									smart_str_appendl(&route_str, itemStr, itemLen );
									smart_str_appendc(&route_str, ')');
								}
								else {
									smart_str_appendl(&route_str, itemStr, itemLen );
								}
								// save it, variable as string key, tmpCount as value

								ZVAL_LONG(&tmp, tmpCount);
								smart_str_0(&variable);
								// copy key and value
								zend_hash_str_add(matches, ZSTR_VAL(variable.s), ZSTR_LEN(variable.s), &tmp); 
							}
							else {
								smart_str_appendl(&route_str, k_route, k_rlen);
								ZVAL_LONG(&tmp, tmpCount);
								zend_hash_str_add(matches, itemStr, itemLen, &tmp); 
							}
						}
						else {
							smart_str_appendc(&route_str, '{');
							smart_str_appendl(&route_str, itemStr, itemLen );
							smart_str_appendc(&route_str, '}');
						}
						continue;
					}
				}

			}
		}
		if (bracketCount == 0) {
			if (ch == '(') {
				parenCount++;
			}
			else if (ch == ')')
			{
				parenCount--;
				if (parenCount == 0) {
					matchCount++;
				}
			}

		}
		if (bracketCount > 0) {
			middle++;
		}
		else {
			if ((parenCount==0) && (prevCh != '\\')) {
				switch(ch) {
					case '.':
					case '+':
					case '|':
					case '#':
						smart_str_appendc(&route_str, '\\');
						break;
					default:
						break;
				}
			}
			smart_str_appendc(&route_str, ch);
			prevCh = ch;
		}
	}
	smart_str_0(&route_str);

	add_next_index_str(return_value, route_str.s);


	ZVAL_ARR(&tmp, matches);
	add_next_index_zval(return_value, &tmp);

	smart_str_free(&route_str);
	smart_str_free(&regex_str);
	smart_str_free(&variable);
}
/* }}}*/

