#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "carray_obj.h"
#include "ucode8.h"
#include "phiz_carray.h"
#include "ext/pcre/php_pcre.h"
#include <zend_smart_str.h>

#ifndef PHIZ_TOMP_H
#include "tom_p.h"
#endif

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


typedef struct _tom_part_tag {
	HashTable*   base;
	int          n;
	bool         aot;
	bool         implicit;
}
tom_part_tag;

HashTable* tag_part_endRef(tom_part_tag* ptag);
HashTable* tag_part_newRef(tom_part_tag* ptag);
zend_string* str_del_char(zend_string* src, char delme, bool* changed );




/** forwards declarations */

int ts_single_match(char32_t c);
void ts_value_zval(toml_stream* oo, zval* ret);
void ts_key_value(toml_stream* oo);

void ts_syntax_error(toml_stream* oo, const char* msg) {
	oo->error = 1;
	oo->errorMsg = strpprintf(0,"TOML syntax: %s", msg);
}

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
int ts_str_captures(toml_stream* oo, zend_string* expr, zend_string* alt)
{
	zval  retval;
	

	if (Z_TYPE(oo->subpats) != IS_NULL) {
		//printf("captures delref %d\n", Z_REFCOUNT(oo->subpats));
		zend_hash_release(Z_ARR(oo->subpats));
    }
	ZVAL_NULL(&oo->subpats);

	ZVAL_NULL(&retval);

	zend_string* src;
	long         offset;

	if (alt) {
		src = alt;
		offset = 0;
	}
	else {
		src = oo->hold;
		offset = oo->index;
	}

	pcre_cache_entry* entry = pcre_get_compiled_regex_cache(expr);

	php_pcre_match_impl(entry, src, &retval, &oo->subpats, 
		/*global*/ 0, /*use flags*/ 1, /*flags*/0, 
		/*offset*/ offset);

	if (Z_TYPE(oo->subpats) == IS_ARRAY) {
		int count = zend_array_count(Z_ARR(oo->subpats));
		printf("%d captures created %d %lx\n",count, Z_REFCOUNT(oo->subpats), Z_ARR(oo->subpats));
		return count;
	}
	return 0;
}
// return count if number of captures > 1
int ts_expr_captures(toml_stream* oo, int exp_index, zend_string* alt) {
	zval *pzval;
	pzval = zend_hash_index_find(oo->map, exp_index);
	if (zval_get_type(pzval) != IS_STRING) {  
		oo->error = 1;
		oo->errorMsg = strpprintf(0, "Invalid expression index %ld", (long)exp_index);
		return 0;
	}
	return ts_str_captures(oo, Z_STR_P(pzval), alt);
}
// return expression id that captures > 1
int ts_firstMatch(toml_stream* oo) {
	int* expSet = oo->expSet;
	zval *pzval;
	int  exp_index;

	while(exp_index = *expSet++) {

		if (ts_expr_captures(oo, exp_index, NULL) > 1) {
			return exp_index;
		}
	}
	return 0;
}
/**
 * Force a zend string from last character lookup
 */


void ts_assign_value(toml_stream* oo, zend_string* val) {
	printf("value release = %d\n", oo->value->gc);
	zend_string_release(oo->value);
	if (!ZSTR_IS_INTERNED(val)) {
		oo->value = zend_string_copy(val);
	}
	else {
		oo->value = val;
	}
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

bool ts_moveExpr(toml_stream* oo, int id) {
	zval*    find;
	zval*    fix;

	if (oo->index < oo->slen) {
		if (ts_expr_captures(oo, id, NULL) > 1) {
			// get value as 2nd capture
			find = zend_hash_index_find(Z_ARR(oo->subpats),1);
			ts_assign_value(oo,Z_STR_P(find));
			// offset position by length of 1st capture
			fix = zend_hash_index_find(Z_ARR(oo->subpats),0);
			oo->index += ZSTR_LEN(Z_STR_P(fix));
			oo->id = tom_AnyChar;
			oo->is_single = 0;
			return true;
		}
	}
	return false;
}

int ts_comment(toml_stream* oo) {
	return ts_moveExpr(oo, tom_Comment);
}

void ts_value_error(toml_stream* oo, char* msg, zend_string* src) {
	oo->error = 1;
	oo->errorMsg = strpprintf(0,"%s : %s", msg, ZSTR_VAL(src));
}


bool ts_float_dot(toml_stream* oo, zend_string* src, zend_string** ret) {
	// continue from subpats
	HashTable* h = Z_ARR(oo->subpats);

	if (zend_array_count(h) < 5) {
		ts_value_error(oo, "Wierd Float Capture", src);
		return false;
	}
	zval* find = zend_hash_index_find(Z_ARR(oo->subpats),4);
	zend_string *pdec = Z_STR_P(find);
	if (ZSTR_LEN(pdec) <= 1) {
		ts_value_error(oo, "Float needs > 1 digit after decimal point", src);
		return false;
	}

	if (ts_expr_captures(oo, tom_Dig_Dig, src))
	{
		ts_value_error(oo, "Float underscore must be between digits", src);
		return false;
	}
	bool changed = false;
	zend_string* stripped = str_del_char(oo->value,'_', &changed);
	*ret = stripped;
	return changed;
}

bool ts_float_exp(toml_stream* oo, zend_string* src, zend_string** ret) {
	// continue from subpats

	if (ts_expr_captures(oo, tom_Float_E, src)) {
		ts_value_error(oo, "Invalid float: Underscore must be between digits", src);
		return false;
	}

	bool changed = false;
	zend_string* strip = str_del_char(src,'_', &changed);
	*ret = strip;

	if (ts_expr_captures(oo, tom_No_0Digit, strip))
	{
		ts_value_error(oo, "Leading zero not allowed", src);
		return changed;
	}
	
	return changed;
}

zend_string* str_del_char(zend_string* src, char delme, bool* changed)
{
	char* sptr = ZSTR_VAL(src);
	int   slen = ZSTR_LEN(src);
	char* eptr = sptr + slen;
	char* bptr = sptr;
	bool change = false;

	while (bptr < eptr) {
			if (*bptr == delme) {
				change = true;
				break;
			}
			bptr++;
		}
	if (change) {
		bptr = sptr;
		smart_str cstr = {0};
		while (bptr < eptr) {
				if (*bptr != delme) {
					smart_str_appendc(&cstr,*bptr);
				}
				bptr++;
			}
			smart_str_0(&cstr);
		*changed = true;
		return cstr.s;
	}
	*changed = false;
	return  src;
}
bool ts_integer(toml_stream* oo, zend_string* src, zend_string** ret) {
	char* sptr = ZSTR_VAL(src);
	int   slen = ZSTR_LEN(src);
	int   change = 0;
	char* eptr = sptr + slen;
	char* bptr;

	if (slen > 0) {
		if ( (*(sptr + slen - 1) == '_')
				 || ((*sptr) == '_') )
		{
			ts_value_error(oo, "Integer underscore must be between digits", src);
			return false;
		}
	}
	bool changed;
	*ret = str_del_char(src,'_',&changed);
	return changed;
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

void ts_mlq_string(toml_stream* oo, zend_string** ret)
{
	int  id;
	bool inloop = true;
	int* old_exp_set = oo->expSet;
	zend_string *fragment;

	smart_str cstr = {0};

	oo->expSet = toml_estr_exp;
	*ret = NULL;
	id = ts_moveNext(oo);
	if (id == tom_NewLine) {
		id = ts_moveNext(oo);
	}
	while(inloop) {
		switch(id) {
			case tom_Quote3:
				inloop = false;
				break;
			case tom_EOS:
				ts_syntax_error(oo,"Missing \"\"\" at end" );
				return;
			case tom_Escape:
				do {
					id = ts_moveNext(oo);
				}
				while (id == tom_Space || id == tom_NewLine || id == tom_Escape);
				break;
			case tom_Space:
				smart_str_appendc(&cstr,' ');
				id = ts_moveNext(oo);
				break;
			case tom_NewLine:
				smart_str_appendc(&cstr,'\n');
				id = ts_moveNext(oo);
			case tom_EscapedChar:

				ts_escape_fragment(oo, &fragment);
				smart_str_append(&cstr, fragment);
				zend_string_release(fragment);
				id = ts_moveNext(oo);
			default:
				smart_str_append(&cstr, oo->value);
				id = ts_moveNext(oo);
				break;
		}
	}
	oo->expSet = old_exp_set;
}

void ts_ml_string(toml_stream* oo, zend_string** ret)
{
	int* old_exp_set = oo->expSet;
	oo->expSet = toml_mlstr_exp;

	int  inloop = 1;
	int  id;

	smart_str cstr = {0};
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
	oo->expSet = toml_estr_exp;
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

void ts_array_type_error(toml_stream* oo, int vtype, int etype) 
{
	oo->error = 1;
	oo->errorMsg = strpprintf(0,"Inline values must be same type %ld <> %ld", vtype, etype);
}

HashTable*
ts_list(toml_stream* oo) {
	HashTable* ret = zend_new_array(4);
	int   value_type = 0;
	int   inloop = 1;
	int   id;
	int   comma = 0;
	zval  element;

	while(inloop) {
		id = ts_peekToken(oo);
		switch(id) {
			case tom_Space:
				ts_moveExpr(oo, tom_Space);
				break;
			case tom_NewLine:
				ts_doneToken(oo);
				break;
			case tom_Hash:
				ts_doneToken(oo);
				ts_comment(oo);
				break;
			case tom_EOS:
				ts_doneToken(oo);
				ts_syntax_error(oo,"Missing ']'");
				return ret;
				break;
			case tom_RSquare:
				ts_doneToken(oo);
			default:
				inloop = false;
				break;
		}
	}
	if (oo->error) {
		return ret;
	}

	while (id != tom_RSquare) {
		if (id == tom_LSquare) {
			// array type is array
			ts_doneToken(oo);

			if (zend_array_count(ret)==0) {
				value_type = IS_ARRAY;
			}
			else {
				ts_array_type_error(oo,value_type,IS_ARRAY);
				break;
			}
			HashTable* inner = ts_list(oo);
			ZVAL_ARR(&element, inner);
			Z_TRY_ADDREF(element);
			zend_hash_next_index_insert(ret, &element);
		}
		else {
			ts_value_zval(oo, &element);
			int ztype = Z_TYPE(element);

			if (zend_array_count(ret)==0) {
				value_type = ztype;
			}
			else if (value_type != ztype) {
				ts_array_type_error(oo,value_type,ztype);
				break;
			}
			Z_TRY_ADDREF(element);
			zend_hash_next_index_insert(ret, &element);
		}
	}
	if (oo->error) {
		return ret;
	}
	inloop = 1;
	while(inloop) {
		id = ts_peekToken(oo);
		switch(id) {
			case tom_Space:
				ts_moveExpr(oo,tom_Space);
				break;
			case tom_NewLine:
				ts_doneToken(oo);
				break;
			case tom_Hash:
				ts_doneToken(oo);
				ts_comment(oo);
				break;
			case tom_Comma:
				if (comma) {
					ts_syntax_error(oo,"No value between commas");
					return ret;
				}
				comma = true;
				ts_doneToken(oo);
				break;
			case tom_EOS:
				ts_syntax_error(oo,"Missing ']'");
				return ret;
			case tom_RSquare:
				ts_doneToken(oo);
			default:
				inloop = false;
				break;

		}
	}
	return ret;

}

int ts_nextNotSpace(toml_stream* oo) {
	int id = ts_moveNext(oo);
	if (id == tom_Space) {
		id = ts_moveNext(oo);
	}
	return id;
}
void ts_inline_table(toml_stream* oo) {
	int id = ts_nextNotSpace(oo);

	if (id != tom_RCurly) {
		ts_key_value(oo);
		id = ts_nextNotSpace(oo);
	}
	while (id == tom_Comma) {
		id = ts_nextNotSpace(oo);
		ts_key_value(oo);
		id = ts_nextNotSpace(oo);
	}
	if (id != tom_RCurly) {
		oo->error = 1;
		oo->errorMsg = strpprintf(0,"Inline table expect '}'");		
	}
}

void ts_key_value(toml_stream* oo) {
	smart_str cstr = {0};
	zend_string     *key;
	ts_key_name(oo, &key);
	zval            rhs;
	HashTable       *oldTable;
	tom_part_tag   *ptag;

	if (zend_hash_exists(oo->table, key)) {
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
		HashTable *list = ts_list(oo);
		ZVAL_ARR(&rhs, list);
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
	add_index_str(&tmp, tom_No_0Digit, wrap_expr_str(cNo_0Digit));
	return re;
}

void ts_init_ts(toml_stream* oo, zend_string* s) {
	ZVAL_NULL(&oo->subpats);

	oo->start = 0;
	oo->end = 0;
	// force a valid (empty) zend_string
	oo->value = zend_string_init("", 0, 0);
	oo->id = 0;
	oo->is_single = 0;
	oo->code = 0;
	oo->error = 0;
	oo->errorMsg = NULL;
	oo->index = 0;
	oo->lineBegin = 0;
	oo->flagLF = 0;
	
	oo->tokenLine = 0;

	if (s) { 
		oo->hold = zend_string_copy(s);
		oo->sptr = ZSTR_VAL(s);
		oo->slen = ZSTR_LEN(s);
	}
	else {
		oo->hold = NULL;
		oo->sptr = NULL;
		oo->slen = 0;
	}
	oo->expSet = toml_key_exp;
	oo->map = toml_make_regx_table();
	oo->root = zend_new_array(4); 
	oo->table_parts = zend_new_array(4); 
	oo->table = oo->root;

}

void ts_destroy_ts(toml_stream* oo)
{
	printf("Destroy ts\n");

	if (oo->hold) {
		zend_string_release(oo->hold);
	}
	if (oo->value) {
		printf("value ref = %d\n", oo->value->gc);
		zend_string_release(oo->value);
	}

	if (Z_TYPE(oo->subpats) != IS_NULL) {
		zend_hash_release(Z_ARR(oo->subpats));
		ZVAL_NULL(&oo->subpats);
	}

	if (oo->root) {
		zend_hash_release(oo->root);
	}

	if (oo->error) {
		if (oo->errorMsg) {
			zend_string_release(oo->errorMsg);
		}
	}
	zend_hash_release(oo->table_parts);
	zend_hash_release(oo->map);
}

int ts_finish_line(toml_stream* oo)
{
	ts_moveExpr(oo, tom_HashComment);
	int id = ts_moveNext(oo);
	if (id != tom_NewLine && id != tom_EOS) {
		ts_syntax_error(oo, "Non-comment at end of line");
	}
	return id;
}


void add_path_part(smart_str* p, zend_string* leaf) {
	smart_str_appendc(p, ']');
	smart_str_append(p,leaf);
	smart_str_appendc(p,']');
	smart_str_0(p);
}
void ts_table_path(toml_stream* oo)
{
	bool isAOT = false;
	bool hitNew = false;
	smart_str path = {0};
	int dotCount = 0;
	int aotLength = 0;

	HashTable* path_ref = oo->root;
	int inloop = 1;
	int parts_ct = 0;
	zend_string     *part_key;
	int id = ts_moveNext(oo);
	tom_part_tag* ptag;

	while(inloop) {
		switch(id) {
			case tom_Hash:
				ts_syntax_error(oo, "Unexpected '#' in path");
				return;
			case tom_Equal:
				ts_syntax_error(oo, "Unexpected '=' in path");
				return;
			case tom_Space:
				id = ts_moveNext(oo);
				break;
			case tom_EOS:
			case tom_NewLine:
				ts_syntax_error(oo, "Incomplete table path");
				return;
			case tom_RSquare:
				if (isAOT) {
					if (aotLength == 0) {
						ts_syntax_error(oo, "AOT Segment cannot be empty");
						return;
					}
					isAOT = false;
					aotLength = 0;
					id = ts_moveNext(oo);
				}
				else {
					inloop = 0;
				}
				break;
			case tom_LSquare:
				if (dotCount < 1 && ZSTR_LEN(path.s) > 0)  {
					ts_syntax_error(oo, "Expected a '.' after path part");
					return;
				}
				if (isAOT) {
					ts_syntax_error(oo, "Too many consecutive [ in path");
					return;
				}
				id = ts_moveNext(oo);
				isAOT = true;
				break;
			case tom_Dot:
				if (dotCount > 0) {
					ts_syntax_error(oo, "Too many consecutive . . in path");
					return;
				}
				dotCount++;
				id = ts_moveNext(oo);
				break;
			case tom_Quote1:
			default:
				
				ts_key_name(oo, &part_key);
				add_path_part(&path, part_key);
				if ((dotCount < 1) && (parts_ct > 0)) {
					ts_syntax_error(oo,"Expected a '.' after path part");
					return;
				}
				dotCount = 0;
				parts_ct += 1;
				if (zend_hash_exists(path_ref, part_key)) {
					zval* part = zend_hash_find(path_ref, part_key);
					if (Z_TYPE_P(part) != IS_ARRAY) {
						ts_syntax_error(oo,"Table path already a key=value");
						return;
					}
					path_ref = Z_ARR_P(part);

					ptag = zend_hash_find_ptr(oo->table_parts, path.s);
					if (ptag==NULL) {
						ts_syntax_error(oo,"Unexpected no path part");
						return;
					}
					if (ptag->aot) {
						aotLength++;
						if (ptag->implicit) {
							path_ref = tag_part_endRef(ptag);
						}
					}
				}
				else {
					hitNew = true;
					if (isAOT) {
						aotLength++;
					}
					HashTable* leaf = zend_new_array(4);
					zval  temp;
					ZVAL_ARR(&temp, leaf);
					zend_hash_add(path_ref, part_key, &temp );
					path_ref = leaf;
					ptag = emalloc(sizeof(tom_part_tag));
					ptag->base = leaf;
					ptag->n = 0;
					ptag->aot = isAOT;
					ptag->implicit = true;
					zend_hash_add_ptr(oo->table_parts, path.s, ptag);

				}
				id = ts_moveNext(oo);
				break;
		} // switch
	} // loop
	if (ZSTR_LEN(path.s)==0) {
		ts_syntax_error(oo, "Table path cannot be empty");
		return;
	}

	if (hitNew) {
		if (ptag->aot) {
			path_ref = tag_part_newRef(ptag);
		}
		else {
			// implicit must be set, and then unset here
			if (ptag->implicit) {
				ptag->implicit = false;
			}
			else {
				ts_syntax_error(oo, "Duplicate table path");
				return;
			}
		}
	}
	else {
		if (ptag->aot) {
			path_ref = tag_part_newRef(ptag);
		}	
		if (ptag->implicit) {
				ptag->implicit = false;
		}
	}
	oo->table = path_ref;
}



bool ts_match_integer(toml_stream* oo, zval* ret, bool* partial) 
{
	printf("match_integer value ref = %d\n", oo->value->gc);
	if (ts_expr_captures(oo, tom_Integer, oo->value)) {
		
		zval* find = zend_hash_index_find(Z_ARR(oo->subpats),1);
		zend_string* match = Z_STR_P(find);
		zend_string* istr;
		if (ZSTR_LEN(oo->value) == ZSTR_LEN(match)) {
			bool release = ts_integer(oo, match, &istr);
			char* endptr;
			long  decval = strtol(ZSTR_VAL(istr),&endptr,10);
			ZVAL_LONG(ret, decval);
			if (release) {
				zend_string_release(istr);
			}
			return true;
		}
		if (!*partial) {
			*partial = true;
		}
	}
	return false;
}

bool ts_match_floatdot(toml_stream* oo, zval* ret, bool* partial) 
{
	if (ts_expr_captures(oo, tom_FloatDot, oo->value)) {
		zval* find = zend_hash_index_find(Z_ARR(oo->subpats),1);
		zend_string* match = Z_STR_P(find);
		zend_string* fstr;
		if (ZSTR_LEN(oo->value) == ZSTR_LEN(match)) {
			ts_float_dot(oo, match, &fstr);
			char* endptr;
			double d = strtod(ZSTR_VAL(fstr),&endptr);
			zend_string_release(fstr);
			ZVAL_DOUBLE(ret, d);
			return true;
		}
		if (!*partial) {
			*partial = true;
		}
	}
	return false;
}

bool ts_match_floatexp(toml_stream* oo, zval* ret, bool* partial) 
{
	if (ts_expr_captures(oo, tom_FloatExp, oo->value)) {
		zval* find = zend_hash_index_find(Z_ARR(oo->subpats),1);
		zend_string* match = Z_STR_P(find);
		zend_string* fstr;
		if (ZSTR_LEN(oo->value) == ZSTR_LEN(match)) {
			ts_float_exp(oo, match, &fstr);
			char* endptr;
			double d = strtod(ZSTR_VAL(fstr),&endptr);
			zend_string_release(fstr);
			ZVAL_DOUBLE(ret, d);
			return true;
		}
		if (!*partial) {
			*partial = true;
		}
	}
	return false;
}

bool ts_match_bool(toml_stream* oo, zval* ret, bool* partial) 
{
	if (ts_expr_captures(oo, tom_Bool, oo->value)) {
		zval* find = zend_hash_index_find(Z_ARR(oo->subpats),1);
		zend_string* match = Z_STR_P(find);
		zend_string* fstr;
		if (ZSTR_LEN(oo->value) == ZSTR_LEN(match)) {
			if (strcmp(ZSTR_VAL(oo->value),"true")==0) {
				ZVAL_TRUE(ret);
			}
			else {
				ZVAL_FALSE(ret);
			}
			return true;
		}
		if (!*partial) {
			*partial = true;
		}
	}
	return false;
}
int call_function_by_ptr(zend_function *fbc, zend_object *obj, zval *retval, uint32_t num_params, zval *params) {
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;

    fci.size = sizeof(fci);
    fci.object = obj;
    fci.retval = retval;
    fci.param_count = num_params;
    fci.params = params;
    fci.named_params = NULL;
    ZVAL_UNDEF(&fci.function_name); // Unused if fcc is provided


    fcc.function_handler = fbc;
    fcc.calling_scope = NULL;       // Appears to be dead
    fcc.called_scope = obj ? obj->ce : fbc->common.scope;
    fcc.object = obj;

    return zend_call_function(&fci, &fcc);
}

bool ts_match_datetime(toml_stream* oo, zval* ret, bool* partial) 
{
	if (ts_expr_captures(oo, tom_DateTime, oo->value)) {
		zval* find = zend_hash_index_find(Z_ARR(oo->subpats),1);
		zend_string* match = Z_STR_P(find);
		zend_string* fstr;
		if (ZSTR_LEN(oo->value) == ZSTR_LEN(match)) {
			static zend_string* dtname = NULL;
			if (dtname == NULL) {
				dtname = zend_string_init("DateTime", sizeof("DateTime")-1, 1);
			}
			zend_class_entry *dtentry = zend_lookup_class(dtname);
			zval obj;
			object_init_ex(&obj, dtentry);
			zend_function *ctor = Z_OBJ_HT(obj)->get_constructor(Z_OBJ(obj));
			int result = call_function_by_ptr(ctor, Z_OBJ(obj), ret, 1, find);
			return true;
		}
		if (!*partial) {
			*partial = true;
		}
	}
	return false;
}

void ts_value_zval(toml_stream* oo, zval* ret) 
{
	ZVAL_NULL(ret);
	zend_string* str = NULL;
	int id = oo->id;

	if (id == tom_Apost1) {
		if (ts_moveExpr(oo, tom_Apost3)) {
			
			ts_ml_string(oo, &str);
		}
		else {
			ts_doneToken(oo);
			ts_literal_string(oo,&str);
		}
	}
	if (id == tom_Quote1) {
		if (ts_moveExpr(oo, tom_Quote3)) {
			ts_mlq_string(oo, &str);
		}
		else {
			ts_doneToken(oo);
			ts_quote_string(oo, &str);
		}
	}
	if (str) {
		ZVAL_STR(ret, str);
		return;
	}

	if (oo->error) {
		return;
	}

	if (!ts_moveExpr(oo, tom_AnyValue)) {
		ts_syntax_error(oo, "Expecting a value");
		return;
	}
	bool isPartial = false;

	if (ts_match_integer(oo, ret, &isPartial)) {
		return;
	}

	if (ts_match_floatexp(oo, ret, &isPartial)) {
		return;
	}

	if (ts_match_floatdot(oo, ret, &isPartial)) {
		return;
	}

	if (ts_match_bool(oo, ret, &isPartial)) {
		return;
	}

	if (ts_match_datetime(oo, ret, &isPartial)) {
		return;
	}

	if (isPartial) {

	}
	if (ts_expr_captures(oo, tom_Integer, oo->value)) {
		zval* find = zend_hash_index_find(Z_ARR(oo->subpats),1);
		ts_assign_value(oo,Z_STR_P(find));
	}
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
            	ts_syntax_error(oo,"Expect <key> = | [<path>] | # comment");
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


