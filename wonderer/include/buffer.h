#ifndef WONDERER_MULTI_BUFFER_H
#define WONDERER_MULTI_BUFFER_H

#ifndef WONDERER_MULTI_BUFFER_SRC
typedef void Buffer;
#endif

#include <GL/glew.h>
#include <stdint.h>

void wondererBufferLoaderInit();
void wondererBufferLoaderTerminate();

Buffer *wondererBufferCreate(GLenum type, uint16_t initial_size,
                             GLenum access_type);

void wondererBufferDestroy(Buffer *buffer);

void wondererBufferFill(Buffer *buffer, void* data, uint16_t data_size);  
void wondererBufferGetData(Buffer *buffer, void *data, uint16_t data_size);

void wondererBufferBind(Buffer* buff);
GLuint wondererBufferGetID(Buffer *buff);
GLenum wondererBufferGetType(Buffer *buff);
GLenum wondererBufferGetAccess(Buffer *buff);

Buffer *wondererBufferGetById(GLuint id);

#endif 
