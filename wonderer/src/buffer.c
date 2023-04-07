#include <GL/glew.h>
#include <malloc.h>
#include <stdint.h>

#include "../include/mem_utils.h"
#include "../include/logger.h"

typedef struct {
   GLuint id;
   uint16_t max_size, curr_size;
   GLenum type, access;
} Buffer;

#define WONDERER_MULTI_BUFFER_SRC
#include "../include/buffer.h"

static dynamic_arr buffer_pool;

void wondererBufferLoaderInit() {
   wondererDynamicArrayInitialize(4, sizeof(Buffer), &buffer_pool);
}

Buffer create_buffer(GLenum type, uint16_t initial_size, GLenum access_type) {

   GLuint id;
   glGenBuffers(1, &id);
   glBindBuffer(type, id);

   glBufferData(type, initial_size, 0, access_type);

   return (Buffer){id, initial_size, 0, type, access_type}; 
}

void wondererBufferBind(Buffer *buffer) {
   if(buffer->id != 0) glBindBuffer(buffer->type, buffer->id);
}

Buffer* wondererBufferCreate(GLenum type, uint16_t initial_size,
                             GLenum access_type) {
   int32_t found = -1;
   Buffer* arr = buffer_pool.arr;

   for(uint16_t i = 0; i < buffer_pool.last; i++)
      if(arr[i].id == 0){
         found = i;
         break;
      }

   if(found != -1){
      arr[found] = create_buffer(type, initial_size, access_type);
      return &arr[found];
   }

   else if(buffer_pool.last == buffer_pool.length){
      wondererDynamicArrayIncreaseSize(2 * buffer_pool.length, &buffer_pool);
      arr = buffer_pool.arr;
   }

   arr[buffer_pool.last] = create_buffer(type, initial_size, access_type);
   return &arr[buffer_pool.last++];
}

void wondererBufferDestroy(Buffer *buffer) {
   glDeleteBuffers(1, &buffer->id);
   *buffer = (Buffer){};
}

void wondererBufferFill(Buffer *buffer, void *data, uint16_t data_size) {
   if(data_size > buffer->max_size){
      WondererError("Cant copy data to #%d buffer (required buffer size %d bytes)",
                    buffer->id, data_size); 
      return;
   }
   if(buffer->id == 0) return;

   wondererBufferBind(buffer);
   glBufferSubData(buffer->type, 0, data_size, data);
   buffer->curr_size = data_size;
}

void wondererBufferGetData(Buffer *buffer, void* data, uint16_t data_size){
   if(data_size > buffer->max_size){
      WondererError("Cant get data that is larger than %d bytes", buffer->max_size);
      return;
   }
   
   wondererBufferBind(buffer);
   glGetBufferSubData(buffer->type, 0, data_size, data);
}

GLuint wondererBufferGetID(Buffer *buffer) { return buffer->id; }
GLenum wondererBufferGetType(Buffer *buff) { return buff->type; }
GLenum wondererBufferGetAccess(Buffer *buff) { return buff->access; }

void wondererBufferLoaderTerminate() {

   Buffer* arr = buffer_pool.arr;
   for(uint16_t i = 0; i < buffer_pool.last; i++)
      glDeleteBuffers(1, &arr[i].id);

   free(buffer_pool.arr);
}

Buffer *wondererBufferGetById(GLuint id) {
   Buffer *arr = buffer_pool.arr;
   for(uint16_t i = 0; i < buffer_pool.last; i++)
      if(arr[i].id == id)
         return &arr[i];

   return NULL;
}
