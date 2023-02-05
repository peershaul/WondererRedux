#pragma once

extern "C"{
	#include "../../include/shader.h"
	#include "../../include/layout.h"
	#include "../../include/texture.h"
}

#include <stdint.h>
#include <GL/glew.h>

#include <vector>

typedef enum {
	WONDERER_DRAW_DATA_INT, 
	WONDERER_DRAW_DATA_FLOAT, 
	WONDERER_DRAW_DATA_MAT4, 
	WONDERER_DRAW_DATA_VEC2, 
	WONDERER_DRAW_DATA_VEC3,
	WONDERER_DRAW_DATA_TEXTURE
} DrawDataType; 


struct Drawer{
	std::vector<int> uniforms;
	std::vector<DrawDataType> types;
	std::vector<uint16_t> lengths;
	std::vector<void*> data_ptrs;
	std::vector<Texture*> textures;

	Shader *shader;
	GLenum data_type_of_indices;
	uint16_t number_of_indices;
	uint8_t textures_start_slot;
	Layout *layout;

	Drawer(const char* vert_path, const char *frag_path, GLenum data_type_of_indices, 
				uint16_t number_of_indices);

	void draw();
	void generate_layout(float *vertices, uint32_t vertices_bytes_size,
	                     void *indices, std::vector<uint8_t> layout_sections);

	void add_uniforms(std::vector<const char*> names, std::vector<DrawDataType> types);
	void fill_texture_slots(std::vector<Texture*> textures);
};


