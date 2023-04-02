#ifndef WONDERER_MULTI_TEXTURE_H
#define WONDERER_MULTI_TEXTURE_H

#ifndef WONDERER_MULTI_TEXTURE_SRC
typedef void Texture;
#endif

#include <stdint.h>
#include <GL/glew.h>
#include <cglm/cglm.h>

void wondererTextureLoaderInit();
void wondererTextureLoaderTerminate();

Texture *wondererTextureCreate();
Texture *wondererTextureCreateWithParameters(GLenum *parameters);

Texture *wondererTextureGet(const char* filepath);
void wondererTextureBind(uint8_t slot, Texture *tex);

void wondererTextureDestroy(Texture *tex);

Texture *wondererTextureGetBound(uint8_t slot);

GLuint wondererTextureGetID(Texture *tex);
void wondererTextureGetDimentions(Texture *tex, ivec2 dimentions);
void wondererTextureSetDimentions(Texture *tex, ivec2 dimentions);

void wondererTextureUploadTexImage(Texture *tex, const char* path);
void wondererTextureSetImageParams(Texture *tex, uint16_t width, uint16_t height, GLenum color_mode);

#endif 
