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
	tom_CommentStuff,
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
const char cCommentStuff[] = "(\\V*)";
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
	zval            value; // string of current token
	zval            subpats; // one or more from preg_match
	int             start;
	int             end;   // view of zend_string* old
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
	int*			expSet;

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

int ts_firstMatch(toml_stream* oo) {
	int* expSet = oo->expSet;
	zval *pzval;
	int  exp_index;
	pcre_cache_entry* entry;
	zval  matches;
	zval  retval;

	ZVAL_NULL(&retval);

	while(exp_index = *expSet++) {

		pzval = zend_hash_index_find(oo->map, exp_index);
		if (zval_get_type(pzval) != IS_STRING) { // TODO:DEBUG
			oo->error = 1;
			oo->errorMsg = strpprintf(0, "Invalid expression index %ld", (long)exp_index);
			return 0;
		}

		entry = pcre_get_compiled_regex_cache(Z_STR_P(pzval));
		Z_TRY_DELREF(oo->subpats);
		ZVAL_NULL(&oo->subpats);

		php_pcre_match_impl(entry, oo->hold, &retval, &oo->subpats, 
			/*global*/ 0, /* use flags */ 1, /* flags */0, /* offset */ oo->index);
		
		if ((Z_TYPE(oo->subpats) == IS_ARRAY)&& zend_array_count(Z_ARR(oo->subpats)) > 1) {
			return exp_index;
		}
	}
	return 0;
}
/**
 * Set the resident token, and update index
 */
int ts_moveNext(toml_stream* oo) {
	uint64_t ch_size;
	int 	 match_id;
	int      flen;
	zval*    find;

	if (oo->flagLF) {
		oo->flagLF = 0;
		oo->tokenLine += 1;
		oo->line = oo->tokenLine;
	}

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
			find = zend_hash_index_find(Z_ARR(oo->subpats),1);
			ZVAL_COPY(&oo->value, find);
			flen = ZSTR_LEN(Z_STR(oo->value));
			/* all captures from first character */
			oo->start = oo->index;
			oo->end = oo->index + flen;
		}
	}
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
	add_index_str(&tmp, tom_CommentStuff, wrap_expr_str(cCommentStuff));
	add_index_str(&tmp, tom_HashComment, wrap_expr_str(cHashComment));

	add_index_str(&tmp, tom_Dig_Dig, wrap_expr_str(cDig_Dig));
	add_index_str(&tmp, tom_Float_E, wrap_expr_str(cFloat_E));

	return re;
}

HashTable* toml_stream_parse(zval* str)
{
	//ZVAL_COPY_VALUE(&ts->hold, str); // no inc ref counter

	toml_stream ts;

	ZVAL_NULL(&ts.value);
	ZVAL_NULL(&ts.subpats);
	ts.start = 0;
	ts.end = 0;
	ts.id = 0;
	ts.is_single = 0;
	ts.code = 0;
	ts.error = 0;
	ts.errorMsg = NULL;
	ts.index = 0;
	ts.lineBegin = 0;
	ts.flagLF = 0;
	
	ts.tokenLine = 0;

	ts.hold = Z_STR_P(str);
	ts.sptr = Z_STRVAL_P(str);
	ts.slen = Z_STRLEN_P(str);
	ts.expSet = toml_key_exp;
	ts.map = toml_make_regx_table();
	ts.root = zend_new_array(4); 
	ts.table = ts.root;

}


