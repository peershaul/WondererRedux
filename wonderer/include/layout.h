#ifndef WONDERER_MULTI_LAYOUT_H
#define WONDERER_MULTI_LAYOUT_H

#ifndef WONDERER_MULTI_LAYOUT_SRC
typedef void Layout;
#endif

#include <stdint.h>
#include <GL/glew.h>

#include "buffer.h"

void wondererLayoutLoaderInit();
void wondererLayoutLoaderTerminate();

Layout *wondererLayoutCreate(uint8_t *sections, uint8_t sections_length,
                             Buffer *vbo, Buffer *ibo);

//Layout *wondererLayoutCreateAll(float *vertices, uint16_t vertices_size, 
//																void *indices, uint16_t indices_size,
//																uint8_t *sections, uint8_t sections_length);

void wondererLayoutDestroy(Layout *layout);
void wondererLayoutBind(Layout *layout);

void wondererLayoutReconfigure(Layout *layout);

GLuint wondererLayoutGetID(Layout *layout);
Layout *wondererLayoutGetBound();
Buffer *wondererLayoutGetBuffer(GLenum buffer_type, Layout *layout);

Layout *wondererLayoutGetById(GLuint id);

#endif
