#ifndef WONDERER_MULTI_TEXTURE_H
#define WONDERER_MULTI_TEXTURE_H

#ifndef WONDERER_MULTI_TEXTURE_SRC
typedef void Texture;
#endif

#include <stdint.h>
#include <GL/glew.h>

void wondererTextureLoaderInit();
void wondererTextureLoaderTerminate();

Texture *wondererTextureCreate();
Texture *wondererTextureGet(const char* filepath);
void wondererTextureBind(uint8_t slot, Texture *tex);

void wondererTextureDestroy(Texture *tex);

Texture *wondererTextureGetBound(uint8_t slot);

GLuint wondererTextureGetID(Texture *tex);

void wondererTextureUploadTexImage(Texture *tex, const char* path);

#endif 
