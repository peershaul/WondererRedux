#ifndef WONDERER_MULTI_OBJ_LOADER 
#define WONDERER_MULTI_OBJ_LOADER 

#ifdef __cplusplus 
extern "C"{
#endif 

#include "drawer.h"

typedef enum {
	WONDERER_OBJ_FORMAT_NONE = -1,
	WONDERER_OBJ_FORMAT_LOCATION = 0,
	WONDERER_OBJ_FORMAT_UV,
	WONDERER_OBJ_FORMAT_NORMAL,
} ObjFormatType;

typedef struct {
	ObjFormatType type;
	int dimentions;
} ObjFormat;

void wondererObjLoad(const char *path, DrawData *dest, ObjFormat *format, 
					 int format_length);

#ifdef __cplusplus
}
#endif 

#endif
