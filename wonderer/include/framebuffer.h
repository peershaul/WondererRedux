#ifndef WONDERER_MULTI_FRAMEBUFFERS_H
#define WONDERER_MULTI_FRAMEBUFFERS_H

#ifndef WONDERER_MULTI_FRAMEBUFFERS_SRC
typedef void FrameBuffer;
#endif 

#include <GL/glew.h> 
#include <cglm/cglm.h>
#include "texture.h"

void wondererFrameBufferLoaderInit();
void wondererFrameBufferLoaderTerminate();

FrameBuffer *wondererFrameBufferCreate(uint16_t width, uint16_t height);
void wondererFrameBufferDestroy(FrameBuffer *fb);

// If sent NULL the program will assume it to be the main framebuffer and act
// accordingly (glDisable(GL_DEPTH_TEST))
void wondererFrameBufferBind(FrameBuffer *buff); 

FrameBuffer *wondererFrameBufferGetBound();

void wondererFrameBufferSetClearColor(FrameBuffer *fb, vec3 color);

GLuint wondererFrameBufferGetFBID(FrameBuffer *buff);
GLuint wondererFrameBufferGetRBID(FrameBuffer *fb);
Texture *wondererFrameBufferGetTexture(FrameBuffer *buff);

#endif 
