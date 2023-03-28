#ifndef WONDERER_STRING_UTILS
#define WONDERER_STRING_UTILS

#include <stdbool.h>
#include <stdint.h>

bool strUtilsStartWith(const char *haystack, const char *needle,
					   uint64_t haystack_length, uint64_t needle_length);



#endif
