#include "str8_obj.h"
#include "ucode8.h"

void phiz_str8_rewind(p_str8 this)
{
	this->next = 0;
	this->current = -1;
	this->ucode = INVALID_CHAR;	
}
void phiz_str8_init(p_str8 this, char* s, long slen) {
	this->str = s;
	this->slen = slen;
	phiz_str8_rewind(this);
}

int phiz_str8_valid(p_str8 this) {
	return ((this->current >= 0) && (this->current < this->slen) 
		&& (this->ucode != INVALID_CHAR)) ? 1 : 0;
}

int phiz_str8_move(p_str8 this) {
	long next = this->next;
	long slen = this->slen - next;

	if (slen > 0) {
		char32_t value = INVALID_CHAR;
		long units = ucode8Fore(this->str + next, slen, &value);
		
		if ((units > 0) && (value != INVALID_CHAR)) {
			this->current = next;
			this->next = next + units;
			this->ucode = value;
			return 1;
		}

	}
	this->current = this->next;
	this->ucode = INVALID_CHAR;
	return 0;
}