#include <GL/glew.h>

#include <stdint.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "../include/buffer.h"
#include "../include/logger.h"
#include "../include/mem_utils.h"

typedef struct {
  GLuint id;
  uint8_t *sections, sections_length;
  Buffer *vbo, *ibo;
} Layout;

#define WONDERER_MULTI_LAYOUT_SRC
#include "../include/layout.h"

static dynamic_arr layout_pool;
static Layout* bound = NULL;

void wondererLayoutLoaderInit() {
  wondererDynamicArrayInitialize(2, sizeof(Layout), &layout_pool);
}

Layout create_layout(uint8_t *sections, uint8_t sections_length, Buffer *vbo,
                     Buffer *ibo) {
  GLuint id;
  glGenVertexArrays(1, &id);

  uint8_t *layout_mem = (uint8_t*) malloc(sections_length * sizeof(uint8_t));
  memcpy(layout_mem, sections, sections_length * sizeof(uint8_t));

  Layout layout = {id, layout_mem, sections_length, vbo, ibo};
  wondererLayoutReconfigure(&layout);

  return layout; 
}


Layout *wondererLayoutCreate(uint8_t *sections, uint8_t sections_length,
                             Buffer *vbo, Buffer *ibo) {
  int32_t found = -1;
  Layout* arr = layout_pool.arr;

  for(uint16_t i = 0; i < layout_pool.last; i++)
    if(arr[i].id == 0){
      found = i;
      break;
    }

  if(found != -1){
    arr[found] = create_layout(sections, sections_length, vbo, ibo);
    return &arr[found];
  }

  else if(layout_pool.last == layout_pool.length){
    wondererDynamicArrayIncreaseSize(2 * layout_pool.length, &layout_pool);
    arr = layout_pool.arr;
  }

  arr[layout_pool.last] = create_layout(sections, sections_length, vbo, ibo);
  return &arr[layout_pool.last++];
}

/* NOT WORKING
Layout *wondererLayoutCreateAll(float *vertices, uint16_t vertices_size,
																void *indices, uint16_t indices_size,
																uint8_t *sections, uint8_t sections_length){
	Buffer *ibo, *vbo;

	vbo = wondererBufferCreate(GL_VERTEX_ARRAY, vertices_size, GL_STATIC_DRAW);
	wondererBufferFill(vbo, vertices, vertices_size);
	
	ibo = wondererBufferCreate(GL_ELEMENT_ARRAY_BUFFER, indices_size, GL_STATIC_DRAW);
	wondererBufferFill(ibo, indices, indices_size);

	int found = -1;
	Layout *arr = layout_pool.arr;

	for(int i = 0; i < layout_pool.last; i++)
		if(arr[i].id == 0){
			found = i;
			break;
		}

	if(found != -1){
		arr[found] = create_layout(sections, sections_length, vbo, ibo);
		return &arr[found];
	}

	else if(layout_pool.last == layout_pool.length){
		wondererDynamicArrayIncreaseSize(2 * layout_pool.length, &layout_pool);
		arr = layout_pool.arr;
	}

	arr[layout_pool.last] = create_layout(sections, sections_length, vbo, ibo);
	return &arr[layout_pool.last++];
}
*/
void wondererLayoutReconfigure(Layout *layout) {
  Layout* last_bound = bound;

  glBindVertexArray(layout->id);

  wondererBufferBind(layout->vbo);
  wondererBufferBind(layout->ibo);

  uint16_t stride = 0, offset = 0;
  for(uint8_t i = 0; i < layout->sections_length; i++){
    glEnableVertexAttribArray(i);
    stride += layout->sections[i];
  }

  for(uint8_t i = 0; i < layout->sections_length; i++){
    glVertexAttribPointer(i, layout->sections[i], GL_FLOAT, GL_FALSE, stride * sizeof(float),
			  (void*)(offset * sizeof(float)));
    offset += layout->sections[i];
  }

  if(last_bound != layout)
    wondererLayoutBind(last_bound);
}

void wondererLayoutBind(Layout *layout) {
  if(layout == NULL)
    glBindVertexArray(0);
  else
    glBindVertexArray(layout->id);

  bound = layout;
}

GLuint wondererLayoutGetID(Layout *layout) { return layout->id; }

Buffer *wondererLayoutGetBuffer(GLenum buffer_type, Layout *layout) {
  switch(buffer_type){
  case GL_ARRAY_BUFFER:
    return layout->vbo;
  case GL_ELEMENT_ARRAY_BUFFER:
    return layout->ibo;
  default:
    WondererWarning("layout id #%d, Invalid buffer type", layout->id);
    return NULL;
  }  
}

Layout *wondererLayoutGetBound(){ return bound; }

void wondererLayoutDestroy(Layout *layout) {
  free(layout->sections);
  glDeleteVertexArrays(1, &layout->id);
  *layout = (Layout){};
}

void wondererLayoutDestroyWithBuffers(Layout *layout){
	free(layout->sections);
	glDeleteVertexArrays(1, &layout->id);
	wondererBufferDestroy(layout->vbo);	
	wondererBufferDestroy(layout->ibo);
	*layout = (Layout){};
}

void wondererLayoutLoaderTerminate() {
  Layout *arr = layout_pool.arr;
  for(uint16_t i = 0; i < layout_pool.last; i++){
    free(arr[i].sections);
    glDeleteVertexArrays(1, &arr[i].id);
  }

  free(layout_pool.arr);

  glBindVertexArray(0);
  bound = NULL;
}

Layout *wondererLayoutGetById(GLuint id) {
  Layout *arr = layout_pool.arr;
  for(uint16_t i = 0; i < layout_pool.last; i++)
    if(arr[i].id == id)
      return &arr[i];

  return NULL;
}
