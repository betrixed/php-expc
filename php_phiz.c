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
#include "phiz_cregx.h"
#include "src/ucode8.h"

extern HashTable* toml_stream_parse(zval *sr);

ZEND_DECLARE_MODULE_GLOBALS(phiz)
static PHP_GINIT_FUNCTION(phiz);
static PHP_GSHUTDOWN_FUNCTION(phiz);

PHPAPI void 
phiz_register_std_class(zend_class_entry ** ppce, 
	char * class_name,  const zend_function_entry* function_list)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY_EX(ce, class_name, strlen(class_name), function_list);
	*ppce = zend_register_internal_class(&ce);
}

PHPAPI void 
phiz_register_child_class(zend_class_entry ** ppce, 
	zend_class_entry *parent, 
	char * class_name, const zend_function_entry* function_list)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY_EX(ce, class_name, strlen(class_name), function_list);
	*ppce = zend_register_internal_class_ex(&ce, parent);
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(phiz)
{
	//REGISTER_INI_ENTRIES();
	PHP_MINIT(phiz_str8)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(phiz_carray)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(phiz_cregx)(INIT_FUNC_ARGS_PASSTHRU);
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


		(&tmp, matches);
	add_next_index_zval(return_value, &tmp);
	//Z_ADDREF_P(return_value);
	//zval_ptr_dtor(&tmp);

	//smart_str_free(&route_str); // don't free
	smart_str_free(&regex_str);
	smart_str_free(&variable);
}
/* }}}*/

/** Only does one character seperator */
void phiz_uncamel(zval* return_value, const zend_string *src, const zend_string *sep)
{
	smart_str uncamel_str = {0};
	const char *marker;
	const char* psep;
	int sep_len = 0;
	unsigned int i;
	int src_len;
	char ch, sepch;

	ZVAL_UNDEF(return_value);
	if (sep!=NULL) {
		psep = ZSTR_VAL(sep);
		sep_len = ZSTR_LEN(sep);
	}
	if (sep_len == 0) {
		sepch = '_';
	}
	else {
		sepch = *psep;
	}

	marker = ZSTR_VAL(src);
	src_len = ZSTR_LEN(src);

	for (i = 0; i < src_len; i++) {
		ch = *marker;
		if (ch == '\0') {
			break;
		}
		if (ch >= 'A' && ch <= 'Z') {
			if (i > 0) {
				smart_str_appendc(&uncamel_str, sepch);
			}
			// lower case
			smart_str_appendc(&uncamel_str, ch + 32);
		} else {
			smart_str_appendc(&uncamel_str, ch);
		}
		marker++;
	}
	smart_str_0(&uncamel_str);

	if (uncamel_str.s) {
		RETURN_STR(uncamel_str.s);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/**
 * Convert dash/underscored texts returning camelized
 * (an optional delimiter can be specified as character-mask as for ltrim)
 */
void phiz_camel(zval* return_value, const zend_string *src, const zend_string *sep)
{
	smart_str camel_str = {0};
	const char* marker;
	const char* psep;
	int   sep_len = 0;
	int i, len, found = 1;
	char ch;

	ZVAL_UNDEF(return_value);
	marker = ZSTR_VAL(src);
	len    = ZSTR_LEN(src);


	if (sep != NULL) {
		psep = ZSTR_VAL(sep);
		sep_len = ZSTR_LEN(sep);
	}
	if (sep_len == 0) {
		psep = "_-";
		sep_len = 2;
	}

	for (i = 0; i < len; i++) {
		ch = marker[i];

		if (memchr(psep, ch, sep_len)) {
			found = 1;
			continue;
		}
		if (found == 1) {
			smart_str_appendc(&camel_str, toupper(ch));
			found = 0;
		} else {
			smart_str_appendc(&camel_str, tolower(ch));
		}
	}

	smart_str_0(&camel_str);

	if (camel_str.s) {
		RETURN_STR(camel_str.s);
	} else {
		RETURN_EMPTY_STRING();
	}
}

ZEND_FUNCTION(str_uncamel) {
	zend_string* src = NULL;
	zend_string* sep = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(src)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(sep)
	ZEND_PARSE_PARAMETERS_END();

	phiz_uncamel(return_value, src, sep );
}

ZEND_FUNCTION(str_camel) {
	zend_string* src = NULL;
	zend_string* sep = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(src)
		Z_PARAM_OPTIONAL
		Z_PARAM_STR(sep)
	ZEND_PARSE_PARAMETERS_END();

	phiz_camel(return_value, src, sep );
}

ZEND_FUNCTION(str_utf8c) {
	zend_string* src = NULL;
	zend_long    offset = 0;
	zval*	   code_ref = 0;
	zend_long        rlen;
	char32_t		ucode;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_STR(src)
		Z_PARAM_LONG(offset)
		Z_PARAM_ZVAL(code_ref)
	ZEND_PARSE_PARAMETERS_END();

	char* s = ZSTR_VAL(src);
	long  slen = ZSTR_LEN(src);
	if (offset >= 0 && offset < slen) {
		rlen = ucode8Fore(s + offset, slen - offset, &ucode);

		ZVAL_LONG(Z_REFVAL_P(code_ref), ucode);
		ZVAL_STRINGL(return_value,s+offset, rlen);
	}
	else {
		ZVAL_LONG(Z_REFVAL_P(code_ref), 0);
		ZVAL_EMPTY_STRING(return_value);
	}
}

ZEND_FUNCTION(toml_parse) {
	zend_string* src = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(src)
	ZEND_PARSE_PARAMETERS_END();

	HashTable* ht = toml_stream_parse(src);

    ZVAL_ARR(return_value, ht);
}