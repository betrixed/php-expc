#ifndef STR8_OBJ_H
#define STR8_OBJ_H

// object structure to iterate UTF8 string

typedef struct _str8_obj* p_str8;

typedef struct _str8_obj {
	char* 				 		str;	
	long                        slen;
	long 		  		 		current; // 0 - indexed offset
	long          		 		next;	   // 0 - indexed offset
	long          		 		ucode; // holds char32_t value
}
str8_obj;


void phiz_str8_init(p_str8 this, char* s, long slen);
int phiz_str8_valid(p_str8 this);
int phiz_str8_move(p_str8 this);
void phiz_str8_rewind(p_str8 this);

#endif