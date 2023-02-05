#ifndef WONDERER_MULTI_DRAWER_H
#define WONDERER_MULTI_DRAWER_H

#include "mem_utils.h"
#include "shader.h"
#include "layout.h"
#include "texture.h"

#include <stdint.h>
#include <GL/glew.h>

typedef enum {
  WONDERER_DRAW_DATA_VEC3,
  WONDERER_DRAW_DATA_VEC2,
  WONDERER_DRAW_DATA_MAT4,
	WONDERER_DRAW_DATA_FLOAT,
  WONDERER_DRAW_DATA_INT,
	WONDERER_DRAW_DATA_TEXTURE,
} DrawDataType; 

typedef struct {
  dynamic_arr uniforms,  // opengl uniforms of type int 
              types,     // wonderer drawer data types 
              lengths,   // length of each array of elements in type uint16_t  
              data_ptrs, // pointers to the uniform data
			  textures;  // textures that you may need
  Shader *shader;
  GLenum data_type_of_indices;
  uint16_t number_of_indices; 
	uint8_t texture_start_slot;
  Layout *layout;
} DrawData;

void wondererDrawerDrawByData(DrawData *data);

void wondererDrawerGenerateLayout(DrawData *data, float *vertices, uint32_t vertices_bytes_size,
		                               void *indices, uint32_t indices_bytes_size, 
																	 uint8_t *layout_sections, uint8_t layout_length); 

void wondererDrawerDestroyDrawData(DrawData *data);
void wondererDrawerDrawByDataInstanced(DrawData *data, uint16_t instance_count);

void wondererDrawerSetUniforms(DrawData *data, char **names, DrawDataType *types, 
															uint16_t *lengths, void **ptrs, uint8_t count);

bool wondererDrawerUploadTextures(DrawData *data, Texture **textures, uint8_t count);

#endif 
