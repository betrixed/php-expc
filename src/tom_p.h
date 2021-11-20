#ifndef PHIZ_TOMP_H
#define PHIZ_TOMP_H

#include <stdint.h>
#include <uchar.h>

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
	HashTable*      table_parts;
}
toml_stream;


typedef struct _phiz_ctoml* pz_tomp;

typedef struct _phiz_ctoml {
	toml_stream 		 ts;
	zend_object          std;
} phiz_ctoml;

extern PHPAPI zend_class_entry *phiz_ce_Tomp;


static inline  pz_tomp phiz_ctoml_from_obj(zend_object *obj)
{
	return (pz_tomp)((char*)(obj) - XtOffsetOf(phiz_ctoml, std));
}

static inline  pz_tomp Z_PHIZ_TOMP_P(zval *zv)
{
	return (pz_tomp)((char*) Z_OBJ_P((zv)) - XtOffsetOf(phiz_ctoml, std));
}

bool ts_match_integer(toml_stream* oo, zval* ret, bool* partial);
bool ts_match_floatexp(toml_stream* oo, zval* ret, bool* partial);
bool ts_match_floatdot(toml_stream* oo, zval* ret, bool* partial);
bool ts_match_datetime(toml_stream* oo, zval* ret, bool* partial);
bool ts_match_bool(toml_stream* oo, zval* ret, bool* partial);

void ts_assign_value(toml_stream* oo, zend_string* val);
void ts_init_ts(toml_stream* oo, zend_string* s);
void ts_destroy_ts(toml_stream* oo);
void ts_handle_error(toml_stream* oo);
void ts_partial_error(toml_stream* oo);
void ts_nomatch_error(toml_stream* oo);
void ts_value_error(toml_stream* oo, char* msg, zend_string* src);
void ts_clear_error(toml_stream* oo);


#endif