#include "../include/drawer.h"

#include "../include/mem_utils.h"
#include "../include/shader.h"
#include "../include/layout.h"
#include "../include/buffer.h"

#include "../include/logger.h"

#include <cglm/cglm.h>
#include <GL/glew.h>
#include <memory.h>

void upload_to_shader_by_type(int uniform, DrawDataType type, uint16_t length,
                              void *data, uint8_t curr_texture) {
  switch(type){

    case WONDERER_DRAW_DATA_INT:
    glUniform1iv(uniform, length, data);
    return;

  case WONDERER_DRAW_DATA_MAT4:
    glUniformMatrix4fv(uniform, length, GL_FALSE, data);
    return;

  case WONDERER_DRAW_DATA_VEC2:{
		glUniform2fv(uniform, length, data);
    return;
  }
  case WONDERER_DRAW_DATA_VEC3:{
    glUniform3fv(uniform, length, data);
    return;
	}

	case WONDERER_DRAW_DATA_FLOAT:
		glUniform1fv(uniform, length, data);
		return;

	case WONDERER_DRAW_DATA_TEXTURE: 
		glUniform1i(uniform, curr_texture);
		return;
  }
}

void prepare_to_draw(DrawData *data) {
  wondererShaderBind(data->shader);

	uint8_t curr_texture = data->texture_start_slot;

  int* uniforms = data->uniforms.arr;
  DrawDataType* types = data->types.arr;
  uint16_t *lengths = data->lengths.arr;
  void** data_ptrs = data->data_ptrs.arr;

	Texture **textures = data->textures.arr;

  for(uint16_t i = 0; i < data->uniforms.last; i++){
		if(types[i] == WONDERER_DRAW_DATA_TEXTURE &&  
							curr_texture - data->texture_start_slot >= data->textures.last) {
			continue;
		}
    WondererGLWrap(upload_to_shader_by_type(uniforms[i], types[i], lengths[i], data_ptrs[i], 
					curr_texture));
		if(types[i] == WONDERER_DRAW_DATA_TEXTURE) curr_texture++;
  }

	for(uint16_t i = 0; i < data->textures.last; i++){
		wondererTextureBind(data->texture_start_slot + i, textures[i]);
	}

	wondererLayoutBind(data->layout);

}

void wondererDrawerDrawByData(DrawData *data) {
  prepare_to_draw(data); 
  WondererGLWrap(glDrawElements(GL_TRIANGLES, data->number_of_indices,
				data->data_type_of_indices, NULL)); 
}

void wondererDrawerDrawByDataInstanced(DrawData *data,
                                       uint16_t instance_count) {
  prepare_to_draw(data);
  WondererGLWrap(glDrawElementsInstanced(GL_TRIANGLES, data->data_type_of_indices,
					 data->data_type_of_indices, NULL, instance_count));
}

void wondererDrawerDestroyDrawData(DrawData *data) {
  free(data->uniforms.arr);
  free(data->types.arr);
  free(data->lengths.arr);
  free(data->data_ptrs.arr);
	free(data->textures.arr);
}

void wondererDrawerGenerateLayout(DrawData *data, float *vertices, uint32_t vertices_size,
																	void *indices, uint32_t indices_size, uint8_t *layout_sections, 
																	uint8_t layout_length){
	Buffer *ibo, *vbo;
	Layout *layout;

	vbo = wondererBufferCreate(GL_ARRAY_BUFFER, vertices_size, GL_STATIC_DRAW);
	wondererBufferFill(vbo, vertices, vertices_size);

	ibo = wondererBufferCreate(GL_ELEMENT_ARRAY_BUFFER, indices_size, GL_STATIC_DRAW);
	wondererBufferFill(ibo, indices, indices_size);

	layout = wondererLayoutCreate(layout_sections, layout_length * sizeof(uint8_t), vbo, ibo);
	
	data->layout = layout;
}

void wondererDrawerSetUniforms(DrawData *data, char **names, DrawDataType *types, 
															uint16_t *lengths, void **ptrs, uint8_t count){

	wondererDynamicArrayInitialize(count, sizeof(int), &data->uniforms);
	wondererDynamicArrayInitialize(count, sizeof(DrawDataType), &data->types);
	wondererDynamicArrayInitialize(count, sizeof(uint16_t), &data->lengths);
	wondererDynamicArrayInitialize(count, sizeof(void*), &data->data_ptrs);
	
	int *arr_uniforms = data->uniforms.arr;
	DrawDataType *arr_types = data->types.arr;
	uint16_t *arr_lengths = data->lengths.arr;
	void **arr_ptrs = data->data_ptrs.arr;

	for(uint16_t i = 0; i < data->uniforms.length; i++)
		arr_uniforms[data->uniforms.last++] = 
			glGetUniformLocation(wondererShaderGetID(data->shader), names[i]);
	
	memcpy(arr_types, types, sizeof(DrawDataType) * count);
	memcpy(arr_ptrs, ptrs, sizeof(void*) * count);
	memcpy(arr_lengths, lengths, sizeof(uint16_t) * count);

	data->types.last = data->types.length;
	data->lengths.last = data->lengths.length;
	data->data_ptrs.last = data->data_ptrs.length;
}

bool wondererDrawerUploadTextures(DrawData *data, Texture **textures, uint8_t count){
	DrawDataType *types = data->types.arr;
	uint8_t tex_count = 0;

	for(uint16_t i = 0; i < data->types.last; i++)
		if(types[i] == WONDERER_DRAW_DATA_TEXTURE)
			tex_count++;

	if(count > tex_count) {
		WondererError("Too many textures, not enough space, please allocate more texture slots");
		return false;
	}

	if(count < tex_count) 
		WondererWarning("Using less textures than allocated");

	wondererDynamicArrayInitialize(tex_count, sizeof(Texture*), &data->textures);
	
	memcpy(data->textures.arr, textures, count * sizeof(Texture*));
	data->textures.last = count;
	return true;
}
