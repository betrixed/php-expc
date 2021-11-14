#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "carray_obj.h"
#include "ucode8.h"
#include "phiz_carray.h"
#include "ext/pcre/php_pcre.h"
#include <zend_smart_str.h>


#define PREG_START_LEN_OFFSET       (1<<10)

enum {
	tom_Bad = 0,
	tom_Equal,
	tom_Bool,
	tom_DateTime,
	tom_EOS,
	tom_Integer, //5
	tom_Quote3,
	tom_Quote1,
	tom_Apost3,
	tom_Apost1,
	tom_NewLine, // 10
	tom_Space,
	tom_LSquare, //12
	tom_RSquare,
	tom_LCurly,
	tom_RCurly,
	tom_Comma,
	tom_Dot,
	tom_BareKey, // 18
	tom_EscapedChar,
	tom_Escape,
	tom_NoEscape,
	tom_FloatExp,
	tom_FloatDot,
	tom_Hash,
	tom_LitString,
	tom_SpacedEqual,
	tom_Comment,
	tom_HashComment,
	tom_AnyValue,
	tom_Dig_Dig,
	tom_No_0Digit,
	tom_Float_E,
	tom_AnyChar,
};

// for regular expression processing

const char cRexBool[] = "(true|false)";
const char cDateTime[] = "(\\d{4}-\\d{2}-\\d{2}(T\\d{2}:\\d{2}:\\d{2}(\\.\\d{6})?(Z|-\\d{2}:\\d{2})?)?)";
const char cFloatExp[] = "([+-]?((\\d_?)+([\\.](\\d_?)*)?)([eE][+-]?(_?\\d_?)+))";
const char cFloatDot[] = "([+-]?((\\d_?)+([\\.](\\d_?)*)))";
const char cInteger[] = "([+-]?(\\d_?)+)";

const char cQuote3[] = "(\"\"\")";
const char cApost3[] = "(\'\'\')";
const char cSpace[] = "(\\h+)";
const char cBareKey[] = "([-A-Z_a-z0-9]+)";

const char cEscapedChar[] = "(\\\\(n|t|r|f|b|\\\"|\\\\|u[0-9A-Fa-f]{4,4}|U[0-9A-Fa-f]{8,8}))";
const char cNoEscape[] = "([^\\x{0}-\\x{19}\\x{22}\\x{5C}]+)";
const char cLitString[] = "([^\\x{0}-\\x{19}\\x{27}]+)";

const char cAnyValue[] = "([^\\s\\]\\},]+)";
const char cSpacedEqual[] = "(\\h*=\\h*)";
const char cComment[] = "(\\V*)";
const char cHashComment[] = "(\\h*#\\V*|\\h*)";

const char cDig_Dig[] = "([^\\d]_[^\\d])|(_$)";
const char cNo_0Digit[] = "(0\\d+)";
const char cFloat_E[] = "([^\\d]_[^\\d])|_[eE]|[eE]_|(_$)";


static int toml_key_exp[] = {tom_Space, tom_BareKey, 
	tom_Integer, 0};

static int toml_value_exp[] = {tom_Bool, tom_DateTime, 
	tom_FloatExp, tom_FloatDot, 
	tom_Integer,0};

static int toml_estr_exp[] = {tom_Space, tom_NoEscape, 
	tom_EscapedChar, tom_Quote3, 0};

static int toml_lstr_exp[] = {tom_LitString, 0};
static int toml_mlstr_exp[] = {tom_LitString, tom_Apost3, 0};



typedef struct _toml_stream {
	zend_string		*value; // string of current token
	uint64_t		start;  // view of current parse
	uint64_t        end; 
	zval            subpats; // captures from preg_match
	uint64_t        id;
	uint64_t        line;
	int        		is_single;
	char32_t   		code;
	int             error;
	zend_string*    errorMsg;
	zend_string*   	hold; // s
	char*		    sptr;
	uint64_t		slen;
	uint64_t	    index;
	int             lineBegin;
	int           	unknownid;
	int           	eolid;
	int           	eosid;
	int           	tokenLine;
	int           	flagLF;
	int*            expSet;

	//  regular expression strings by enum
	HashTable*		map;
	// 	build up parsed data		
	HashTable*		root;
	HashTable*		table;
}
toml_stream;


/** forwards declarations */

int ts_single_match(char32_t c);
/**
 * Fetch without updating the streams index
 */
int 
ts_peekChar(toml_stream* oo)  {
	int64_t 	remains = oo->slen - oo->index;
	uint64_t 	ch_size;

	if (remains > 0) {
		ch_size = ucode8Fore(oo->sptr + oo->index, remains, &oo->code);
		if (oo->code != INVALID_CHAR) {
			oo->start = oo->index;
			oo->end = oo->start + ch_size;
			return ch_size;
		}
	}
	oo->id = tom_EOS;
	oo->is_single = 1;
	oo->start = oo->index;
	oo->end = oo->index;
	return 0;
}


void ts_checkLF(toml_stream* oo) {
	if (oo->code == 13) {
		oo->index += 1;
		ts_peekChar(oo);
	}
	if (oo->code == 10) {
		oo->flagLF = true;
		oo->id = tom_NewLine;
		oo->line = oo->tokenLine;
		oo->is_single = true;
		oo->lineBegin = oo->index + 1;
		return;
	}
	oo->error = 1;
	oo->errorMsg = strpprintf(0,"Invalid character ts_checkLF %lx", (long)oo->code );
}


int ts_peekToken(toml_stream* oo) {
	int fit;
	int csize = ts_peekChar(oo);
	if (csize == 0) {
		oo->line = oo->tokenLine;
	}
	else if (oo->code < 20) {
		ts_checkLF(oo);
	}
	else {
		oo->line = oo->tokenLine;
		oo->is_single = 0;
		if (csize == 1) {
			fit = ts_single_match(oo->code);
			if (fit) {
				oo->id = fit;
				oo->is_single = 1;
			}
			else {
				oo->id = tom_EOS;
			}
		}
		else {
			oo->id = tom_EOS;
		}
	}
	oo->line = oo->tokenLine;
	if (oo->error) {
		return 0;
	}
	else {
		return oo->id;
	}
}

// return count if number of captures > 1
int ts_str_captures(toml_stream* oo, zend_string* expr)
{
	zval  retval;
	pcre_cache_entry* entry = pcre_get_compiled_regex_cache(expr);
	Z_TRY_DELREF(oo->subpats);
	ZVAL_NULL(&oo->subpats);

	ZVAL_NULL(&retval);
	php_pcre_match_impl(entry, oo->hold, &retval, &oo->subpats, 
		/*global*/ 0, /*use flags*/ 1, /*flags*/0, 
		/*offset*/ oo->index);
	
	if (Z_TYPE(oo->subpats) == IS_ARRAY) {
		return zend_array_count(Z_ARR(oo->subpats));
	}
	return 0;
}
// return count if number of captures > 1
int ts_expr_captures(toml_stream* oo, int exp_index) {
	zval *pzval;
	pzval = zend_hash_index_find(oo->map, exp_index);
	if (zval_get_type(pzval) != IS_STRING) {  
		oo->error = 1;
		oo->errorMsg = strpprintf(0, "Invalid expression index %ld", (long)exp_index);
		return 0;
	}
	return ts_str_captures(oo, Z_STR_P(pzval));
}
// return expression id that captures > 1
int ts_firstMatch(toml_stream* oo) {
	int* expSet = oo->expSet;
	zval *pzval;
	int  exp_index;

	while(exp_index = *expSet++) {

		if (ts_expr_captures(oo, exp_index) > 1) {
			return exp_index;
		}
	}
	return 0;
}
/**
 * Force a zend string from last character lookup
 */
int ts_string_zend(toml_stream* oo, zend_string **ret) {
	smart_str zstr = {0};
	int slen = oo->end - oo->start;
	if ((slen) > 0) {
		smart_str_appendl(&zstr, oo->sptr + oo->start, slen);
	}
	smart_str_0(&zstr);
	(*ret) = zstr.s;
}

void ts_assign_value(toml_stream* oo, zend_string* val) {
	zend_string_release(oo->value);
	oo->value = zend_string_copy(val);
}

void ts_doneToken(toml_stream* oo)
{
	int id = oo->id;

	if (oo->flagLF) {
		oo->flagLF = 0;
		oo->tokenLine += 1;
	}
	oo->line = oo->tokenLine;
	if (id == tom_EOS) {
		return;
	}
	if (id == tom_NewLine) {
		oo->index++;
		return;
	}
	oo->index += ZSTR_LEN(oo->value);
}


int ts_moveNext(toml_stream* oo) {
	uint64_t ch_size;
	int 	 match_id;
	zval*    find;

	if (oo->flagLF) {
		oo->flagLF = 0;
		oo->tokenLine += 1;
		oo->line = oo->tokenLine;
	}
	// check next character exists
	ch_size = ts_peekChar(oo);
	if (ch_size==0) {
		oo->id = tom_EOS;
	}
	else if (oo->code < 20) {
		ts_checkLF(oo);
		oo->index++;
	}
	else {
		match_id = ts_firstMatch(oo);
		if (match_id > 0) {
			oo->id = match_id;
			// increment position by length of 2nd capture
			find = zend_hash_index_find(Z_ARR(oo->subpats),1);
			ts_assign_value(oo,Z_STR_P(find));
			oo->index += ZSTR_LEN(oo->value);
			/* all captures from first character */
		}
	}
	oo->line = oo->tokenLine;
	return oo->id;
}

// return token id for short list of single ASCII/unicode values
int ts_single_match(char32_t c) {
	switch(c) {
		case '=': return tom_Equal;
		case '[': return tom_LSquare;
		case ']': return tom_RSquare;
		case '.': return tom_Dot;
		case ',': return tom_Comma;
		case '{': return tom_LCurly;
		case '}': return tom_RCurly;
		case '\'': return tom_Apost1;
		case '\"': return tom_Quote1;
		case '#': return tom_Hash;
		case '\\': return tom_Escape;
		case ' ': return tom_Space;
		case '\t': return tom_Space;
		case '\f': return tom_Space;
		case '\b': return tom_Space;
		default: return 0;
	}
}

int ts_moveExpr(toml_stream* oo, int id) {
	zval*    find;
	zval*    fix;

	if (oo->index < oo->slen) {
		if (ts_expr_captures(oo, id) > 1) {
			// get value as 2nd capture
			find = zend_hash_index_find(Z_ARR(oo->subpats),1);
			ts_assign_value(oo,Z_STR_P(find));
			// offset position by length of 1st capture
			fix = zend_hash_index_find(Z_ARR(oo->subpats),0);
			oo->index += ZSTR_LEN(Z_STR_P(fix));
			oo->id = tom_AnyChar;
			oo->is_single = 0;
			return 1;
		}
	}
	return 0;
}

int ts_comment(toml_stream* oo) {
	return ts_moveExpr(oo, tom_Comment);
}

void ts_integer(toml_stream* oo, zend_string* src, zend_string** ret) {
	char* sptr = ZSTR_VAL(src);
	int   slen = ZSTR_LEN(src);
	int   change = 0;
	char* eptr = sptr + slen;
	char* bptr;

	if (slen > 0) {
		if ( (*(sptr + slen - 1) == '_')
				 || ((*sptr) == '_') )
		{
			oo->error = 1;
			oo->errorMsg = strpprintf(0,"Integer underscore must be between digits %s", src);
		}
		bptr = sptr;
		while (bptr < eptr) {
			if (*bptr == '_') {
				change = 1;
			}
			bptr++;
		}
		if (change) {
			smart_str	  cstr = {0}; 
			bptr = sptr;
			while (bptr < eptr) {
				if (*bptr != '_') {
					smart_str_appendc(&cstr,*bptr);
				}
				bptr++;
			}
			smart_str_0(&cstr);
			(*ret) = cstr.s;
			return;
		}
		(*ret) = zend_string_copy(src);
		return;
	}
}

/* string contains '\' followed by escaped character specifier */
void ts_escape_fragment(toml_stream* oo, 
	zend_string** ret) {
	smart_str	cstr = {0};
	char result[8];
	int  rlen;

	char* sptr = ZSTR_VAL(oo->value);
	int   slen = ZSTR_LEN(oo->value);

	char val = *(sptr+1);
	switch(val) {
		case 'n':
			smart_str_appendl(&cstr, "\n",sizeof("\n")-1);
			break;
		case 't':
			smart_str_appendc(&cstr, '\t');
			break;
		case 'r':
			smart_str_appendc(&cstr, '\r');
			break;
		case 'b':
			smart_str_appendc(&cstr, '\n');
			break;	
		case 'f':
			smart_str_appendc(&cstr, '\f');
			break;
		case '"':
			smart_str_appendc(&cstr, '"');
			break;
		case '\\':
			smart_str_appendc(&cstr, '\\');
			break;
		case 'u':
			rlen = hex_str8(sptr+2,4,&result);
			smart_str_appendl(&cstr,result, rlen);
			break;
		case 'U':
			rlen = hex_str8(sptr+2,8,&result);
			smart_str_appendl(&cstr,result, rlen);
			break;
		default:
			oo->error = 1;
			oo->errorMsg = strpprintf(0,"invalid escaped character %s",val);
			break;	
	}
	smart_str_0(&cstr);
	(*ret) = cstr.s;
	return;

}

void ts_ml_string(toml_stream* oo, zend_string** ret)
{
	int* old_exp_set = oo->expSet;
	int  inloop = 1;
	int  id;

	smart_str cstr = {0};

	oo->expSet = toml_mlstr_exp;

	id = ts_moveNext(oo);
	if (id == tom_NewLine) {
		id = ts_moveNext(oo);
	}
	while(inloop) {
		switch(id) {
		case tom_NewLine:
			smart_str_appendc(&cstr, '\n');
			id = ts_moveNext(oo);
			break;
		case tom_Apost3:
			inloop = 0;
			break;
		case tom_EOS:
			oo->error = 1;
			oo->errorMsg = strpprintf(0,"Missing end '''");
			inloop = 0;
			break;
		default:
			smart_str_append(&cstr, oo->value);
			id = ts_moveNext(oo);
			break;
		}
	}
	*ret = cstr.s;
	oo->expSet = old_exp_set;
}

void ts_quote_string(toml_stream* oo, zend_string** ret) 
{
	int* old_exp_set = oo->expSet;
	oo->expSet = old_exp_set;
	smart_str cstr = {0};

	int id = ts_moveNext(oo);
	while( id != tom_Quote1 ) {
		if (id == tom_NewLine || id == tom_EOS || id == tom_Escape) 
		{
			oo->error = 1;
			oo->errorMsg = strpprintf(0,"Missing end quote");
			break;
		}
		else if (id == tom_EscapedChar) {

			zend_string *temp;
			ts_escape_fragment(oo, &temp);
			smart_str_append(&cstr, temp);
			zend_string_release(temp);
		}
		else {
			id = ts_moveNext(oo);
		}
	}

	*ret = cstr.s;
	oo->expSet = old_exp_set;
}


void ts_literal_string(toml_stream* oo, zend_string** ret)
{
	smart_str cstr = {0};

	int id = ts_peekToken(oo);

	while (id != tom_Apost1) {
		if (id == tom_NewLine || id == tom_EOS) {
			oo->error = 1;
			oo->errorMsg = strpprintf(0,"Value without end quote (')");
			break;
		}
		if (ts_moveExpr(oo, tom_LitString)) {
			smart_str_append(&cstr, oo->value);
		}
		else {
			oo->error = 1;
			oo->errorMsg = strpprintf(0,"Invalid string value");
			break;
		}
		ts_doneToken(oo);
	}
}

void ts_key_name(toml_stream* oo, zend_string** ret) {
	int id = oo->id;
	switch(id) {
		case tom_BareKey: 
			*ret = zend_string_copy(oo->value);
			break;
		case tom_Quote1:
			ts_quote_string(oo,ret);
			break;
		case tom_Apost1:
			ts_literal_string(oo,ret);
			break;
		case tom_Integer:
			ts_integer(oo, oo->value, ret);
			break;
		default:
			oo->error = 1;
			oo->errorMsg = strpprintf(0,"Improper Key");
			break;
	}
}

void ts_list(toml_stream* oo, zval* ret) {

}

void ts_inline_table(toml_stream* oo) {

}

void ts_key_value(toml_stream* oo) {
	smart_str cstr = {0};
	zend_string     *key;
	ts_key_name(oo, &key);
	zval            rhs;
	HashTable       *oldTable;

	if (zend_hash_str_exists(oo->table, ZSTR_VAL(key), ZSTR_LEN(key))) {
		oo->error = 1;
		oo->errorMsg = strpprintf(0,"Duplicate key %s", ZSTR_VAL(key));
		return;
	}	
	if (!ts_moveExpr(oo, tom_SpacedEqual)) {
		oo->error = 1;
		oo->errorMsg = strpprintf(0,"Expect <key> = ");
		return;
	}
	int id = ts_peekToken(oo);

	if (id == tom_LSquare) {
		ts_doneToken(oo);
		ts_list(oo, &rhs);
	}
	else if (id == tom_LCurly) {
		ts_doneToken(oo);
		oldTable = oo->table;
		oo->table = zend_new_array(4);
		ZVAL_ARR(&rhs, oo->table);
		ts_inline_table(oo);
		oo->table = oldTable;
	}
	else {
		ZVAL_STR(&rhs, oo->value);
	}
	zend_hash_add(oo->table, key, &rhs);
	zend_string_release(key);
}
static zend_string* wrap_expr_str(const char* expr)
{
	smart_str	  cstr = {0};

	smart_str_appendl(&cstr,"/\\G",3);
	smart_str_appends(&cstr, expr);
	smart_str_appendl(&cstr,"/u",2);
	smart_str_0(&cstr);
	return cstr.s;
}

HashTable* toml_make_regx_table()
{
	HashTable* re = zend_new_array(tom_AnyChar);

	zval tmp;
	ZVAL_ARR(&tmp,re);


	add_index_str(&tmp, tom_Bool, wrap_expr_str(cRexBool));
	add_index_str(&tmp, tom_DateTime, wrap_expr_str(cDateTime));
	add_index_str(&tmp, tom_FloatExp, wrap_expr_str(cFloatExp));
	add_index_str(&tmp, tom_FloatDot, wrap_expr_str(cFloatDot));
	add_index_str(&tmp, tom_Integer, wrap_expr_str(cInteger));

	add_index_str(&tmp, tom_Quote3, wrap_expr_str(cQuote3));
	add_index_str(&tmp, tom_Apost3, wrap_expr_str(cApost3));
	add_index_str(&tmp, tom_Space, wrap_expr_str(cSpace));
	add_index_str(&tmp, tom_BareKey, wrap_expr_str(cBareKey));

	add_index_str(&tmp, tom_EscapedChar, wrap_expr_str(cEscapedChar));
	add_index_str(&tmp, tom_NoEscape, wrap_expr_str(cNoEscape));
	add_index_str(&tmp, tom_LitString, wrap_expr_str(cLitString));

	add_index_str(&tmp, tom_AnyValue, wrap_expr_str(cAnyValue));
	add_index_str(&tmp, tom_SpacedEqual, wrap_expr_str(cSpacedEqual));
	add_index_str(&tmp, tom_Comment, wrap_expr_str(cComment));
	add_index_str(&tmp, tom_HashComment, wrap_expr_str(cHashComment));

	add_index_str(&tmp, tom_Dig_Dig, wrap_expr_str(cDig_Dig));
	add_index_str(&tmp, tom_Float_E, wrap_expr_str(cFloat_E));

	return re;
}

void ts_init_ts(toml_stream* oo, zend_string* s) {
	ZVAL_NULL(&oo->subpats);

	oo->start = 0;
	oo->end = 0;
	// force a valid (empty) zend_string
	ts_string_zend(oo, &oo->value);
	oo->id = 0;
	oo->is_single = 0;
	oo->code = 0;
	oo->error = 0;
	oo->errorMsg = NULL;
	oo->index = 0;
	oo->lineBegin = 0;
	oo->flagLF = 0;
	
	oo->tokenLine = 0;

	oo->hold = s;
	oo->sptr = ZSTR_VAL(s);
	oo->slen = ZSTR_LEN(s);
	oo->expSet = toml_key_exp;
	oo->map = toml_make_regx_table();
	oo->root = zend_new_array(4); 
	oo->table = oo->root;
}

void ts_finish_line(toml_stream* oo)
{

}

void ts_table_path(toml_stream* oo)
{

}
void ts_destroy_ts(toml_stream* oo)
{
	if (oo->value) {
		zend_string_release(oo->value);
	}
	Z_TRY_DELREF(oo->subpats);
	if (oo->root) {
		zend_hash_release(oo->root);
	}
	zend_hash_release(oo->map);
}

HashTable*
ts_parse_string(toml_stream* oo, zend_string *s) {
	HashTable* ret;

	ts_init_ts(oo, s);
	int id = ts_moveNext(oo);

	while ( id != tom_EOS) {
		switch(id) {
			case tom_Hash:
				ts_comment(oo);
				break;
			case tom_Space:
			case tom_NewLine:
				id = ts_moveNext(oo);
				break;
	        case tom_Quote1:
            case tom_BareKey:
            case tom_Apost1:
            case tom_Integer:
            	ts_key_value(oo);
            	ts_finish_line(oo);
            	break;
            case tom_LSquare:
            	ts_table_path(oo);
            	ts_finish_line(oo);
            	break;
            default:
            	oo->error = 1;
            	oo->errorMsg = strpprintf(0,"Expect <key> = | [<path>] | # comment");
            	break;
		}
		if (oo->error) {
			zend_string* msg = oo->errorMsg;
			oo->errorMsg = NULL;
			ret = oo->root;
			oo->root = NULL;
			ts_destroy_ts(oo);
			zend_throw_error_exception(zend_ce_error_exception, msg, 0,E_ERROR);
			return ret;
		}
	}
	ret = oo->root;
	oo->root = NULL;
	ts_destroy_ts(oo);
	return ret;
}
HashTable* toml_stream_parse(zend_string* str)
{
	//ZVAL_COPY_VALUE(&ts->hold, str); // no inc ref counter

	toml_stream oo;
	ts_parse_string(&oo, str);

	
}


