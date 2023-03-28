#include "../include/str_utils.h"

#include <stdbool.h>
#include <string.h>
#include <stdint.h> 

bool strUtilsStartWith(const char *haystack, const char *needle, 
					   uint64_t haystack_length, uint64_t needle_length){

	if(needle_length > haystack_length) return false;

	for(uint64_t i = 0; i < needle_length; i++)
		if(haystack[i] != needle[i])
			return false;

	return true;
}
