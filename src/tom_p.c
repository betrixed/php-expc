#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_exceptions.h"
#include "carray_obj.h"
#include "ucode8.h"
#include "phiz_carray.h"

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

const char cRexBool[] = "^(true|false)";
const char cDateTime[] = "^(\\d{4}-\\d{2}-\\d{2}(T\\d{2}:\\d{2}:\\d{2}(\\.\\d{6})?(Z|-\\d{2}:\\d{2})?)?)";
const char cFloatExp[] = "^([+-]?((\\d_?)+([\\.](\\d_?)*)?)([eE][+-]?(_?\\d_?)+))";
const char cFloatDot[] = "^([+-]?((\\d_?)+([\\.](\\d_?)*)))";
const char cInteger[] = "^([+-]?(\\d_?)+)";

const char cQuote3[] = "^(\"\"\")";
const char cApost3[] = "^(\'\'\')";
const char cSpace[] = "^(\\h+)";
const char cBareKey[] = "^([-A-Z_a-z0-9]+)";

const char cEscapedChar[] = "^(\\\\(n|t|r|f|b|\\\"|\\\\|u[0-9A-Fa-f]{4,4}|U[0-9A-Fa-f]{8,8}))";
const char cNoEscape[] = "^([^\\x{0}-\\x{19}\\x{22}\\x{5C}]+)";
const char cLitString[] = "^([^\\x{0}-\\x{19}\\x{27}]+)";

const char cAnyValue[] = "^([^\\s\\]\\},]+)";
const char cSpacedEqual[] = "^(\\h*=\\h*)";
const char cCommentStuff[] = "^(\\V*)";
const char cHashComment[] = "^(\\h*#\\V*|\\h*)";

const char cDig_Dig[] = "^([^\\d]_[^\\d])|(_$)";
//const char cNo_0Digit = "^(0\\d+)";
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


typedef struct  _toml_token {
	uint64_t        start; // start of string segment
	uint64_t        end;   // one plus last character

	uint64_t        id;
	uint64_t        line;
	int        		is_single;
	char32_t   		ucp;

}
toml_token;


typedef struct _toml_stream {
	toml_token*     token;
	char*		    sptr;
	uint64_t		slen;
	uint64_t	    index;

	int           	unknownid;
	int           	eolid;
	int           	eosid;
	int           	tokenLine;
	int           	flagLF;
	int*			expSet;

	//  regular expression strings by enum
	HashTable*		regx;
	// 	build up parsed data		
	HashTable*		root;
	HashTable*		table;
}
toml_stream;




toml_token* 
init_toml_token(toml_token* tt) {

	tt->start = 0;
	tt->end = 0;
	tt->id = 0;
	tt->line = 0;
	tt->is_single = 0;
	tt->ucp = INVALID_CHAR;	

	return tt;
}

toml_token* 
create_toml_token()
{
	return init_toml_token((toml_token*)emalloc(sizeof(toml_token)));
}

typedef garray token_array;

static void init_toml_tokens(p_garray oo, long from, long to) {
	toml_token** begin = (toml_token**) oo->head.elements + from;
	toml_token** end = (toml_token**) oo->head.elements + to;
	while (begin != end) {
		 *begin = NULL;
		 begin++;
	}

}



static void copy_toml_tokens
 (p_garray oo, long offset, p_garray pp, long begin, long end)
{
	toml_token** to = (toml_token**) oo->head.elements + offset;
	toml_token** bgp = (toml_token**) pp->head.elements + begin;
	toml_token** ep = (toml_token**) pp->head.elements + end;

	while (bgp != ep) {
		*to++ = *bgp++;
	}
}



token_array*
new_array_token(token_array* ta) {
	gen_array_fntab* tab = emalloc(sizeof(gen_array_fntab));

	tab->esize = sizeof(toml_token*);
	tab->ename = "toml_token";
	tab->ctdt = 0;
	tab->etype = 100;
	tab->init_elems = init_toml_tokens;
	tab->dtor_elems = NULL;
	tab->copy_elems = copy_toml_tokens;

	ta->fntab = tab;

	return ta;
}

inline toml_token** token_array_elem(token_array* ta, long ix)
{
	return ((toml_token**)ta->head.elements + ix);
}

toml_token* 
token_push_back(token_array* ta, toml_token* tt) {
	long offset = ta->head.size;
	gen_resize(ta, offset+1);
	*token_array_elem(ta,offset) = tt;
	return  tt;
}

/**
 * Fetch without updating the streams index
 */
int ts_peekChar(const toml_stream* ts, toml_token* tt) {
	int64_t 	remains = ts->slen - ts->index;
	uint64_t 	ch_size;

	if (remains > 0) {
		ch_size = ucode8Fore(ts->sptr + ts->index, remains, &tt->ucp);
		if (tt->ucp != INVALID_CHAR) {
			tt->start = ts->index;
			tt->end = tt->start + ch_size;
			return ch_size;
		}
	}
	tt->id = tom_EOS;
	tt->is_single = 1;
	tt->start = tt->end;
	return 0;
}

void ts_checkLineFeed(toml_stream* oo, toml_token* tt) {
	if (tt->ucp == 13) {
		oo->index += 1;
		ts_peekChar(oo,tt);
	}
	if (tt->ucp == 10) {
		oo->flagLF = true;
		tt->id = oo->eolid;
		tt->line = oo->tokenLine;
		tt->is_single = true;
		return;
	}
	zend_throw_exception_ex(phiz_ce_RuntimeException, 0,
			"Invalid in Toml checkLineFeed %ld", (long)tt->ucp);
}


int ts_firstMatch(toml_stream* oo) {
	int* expSet = oo->expSet;
	zval *pzval;
	int  exp_index;

	while(exp_index = *expSet++) {

		pzval = zend_hash_index_find(oo->regx, exp_index);
		if (Z_TYPE(pzval) !== IS_STRING) { // TODO:DEBUG
			zend_throw_exception_ex(phiz_ce_RuntimeException, 0,
			"Invalid expression index %ld", (long)exp_index);
}
		}
	}
}
/**
 * Set the resident token, and update index
 */
int ts_moveNextId(toml_stream* oo) {
	uint64_t ch_size;
	uint64_t match_id;

	toml_token* tok = oo->token;
	if (oo->flagLF) {
		oo->flagLF = 0;
		oo->tokenLine += 1;
		tok->line = oo->tokenLine;
	}

	ch_size = ts_peekChar(oo,tok);
	if (ch_size==0) {
		tok->id = tom_EOS;
	}
	else if (tok->ucp < 20) {
		ts_checkLineFeed(oo, tok);
		oo->index++;
	}
	else {
		match_id = ts_firstMatch(oo);
	}
}

// return token id for short list of single ASCII/unicode values
int64_t toml_singles_table(char32_t c) {
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

HashTable* toml_make_regx_table()
{
	HashTable* re = zend_new_array(tom_AnyChar);

	zval tmp;
	ZVAL_ARR(&tmp,re);

	add_index_string(&tmp, tom_Bool, cRexBool);
	add_index_string(&tmp, tom_DateTime, cDateTime);
	add_index_string(&tmp, tom_FloatExp, cFloatExp);
	add_index_string(&tmp, tom_FloatDot, cFloatDot);
	add_index_string(&tmp, tom_Integer, cInteger);

	add_index_string(&tmp, tom_Quote3, cQuote3);
	add_index_string(&tmp, tom_Apost3, cApost3);
	add_index_string(&tmp, tom_Space, cSpace);
	add_index_string(&tmp, tom_BareKey, cBareKey);

	add_index_string(&tmp, tom_EscapedChar, cEscapedChar);
	add_index_string(&tmp, tom_NoEscape, cNoEscape);
	add_index_string(&tmp, tom_LitString, cLitString);

	add_index_string(&tmp, tom_AnyValue, cAnyValue);
	add_index_string(&tmp, tom_SpacedEqual, cSpacedEqual);
	add_index_string(&tmp, tom_CommentStuff, cCommentStuff);
	add_index_string(&tmp, tom_HashComment, cHashComment);

	add_index_string(&tmp, tom_Dig_Dig, cDig_Dig);
	add_index_string(&tmp, tom_Float_E, cFloat_E);

	return re;
}

HashTable*
 	toml_stream_parse(toml_stream* ts, char* strp, int slen)
{
	ts->sptr = strp;
	ts->slen = slen;
	ts->flagLF = 0;
	ts->index = 0;
	ts->eosid = tom_EOS;
	ts->eolid = tom_NewLine;
	ts->unknownid = tom_AnyChar;
	ts->tokenLine = 0;
	ts->token = create_toml_token();
	ts->root = zend_new_array(4); 
	ts->table = ts->root;
	ts->expSet = toml_key_exp;

	ts->regx = toml_make_regx_table();

}


