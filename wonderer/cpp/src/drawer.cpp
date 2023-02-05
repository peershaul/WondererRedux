#include "../include/drawer.hpp"

extern "C"{
#include "../../include/shader.h"
#include "../../include/layout.h"
#include "../../include/buffer.h"
#include "../../include/texture.h"
}

#include <vector>

uint8_t data_type_size(GLenum type){
	switch(type){
		case GL_UNSIGNED_BYTE:
		case GL_BYTE:
			return sizeof(uint8_t);

		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
			return sizeof(uint16_t);

		case GL_UNSIGNED_INT:
		case GL_INT:
				return sizeof(uint32_t);

		default:
				return 0;
	}
}

Drawer::Drawer(const char* vert_path, const char *frag_path, GLenum data_type_of_indices, 
							uint16_t number_of_indices){

	shader = wondererShaderGet(vert_path, frag_path);
	this->data_type_of_indices = data_type_of_indices;
	this->number_of_indices = number_of_indices;

	this->uniforms = {};
	this->textures = {};
	this->lengths = {};
	this->data_ptrs = {};
	this->types = {};

	this->textures_start_slot = 0;
	this->layout = nullptr;

}

void Drawer::generate_layout(float *vertices, uint32_t vertices_bytes_size, 
			                       void *indices, std::vector<uint8_t> layout_sections){
	Buffer *ibo, *vbo;

	uint32_t indices_size = data_type_size(data_type_of_indices) * number_of_indices; 
	
	vbo = wondererBufferCreate(GL_ARRAY_BUFFER, vertices_bytes_size, GL_STATIC_DRAW);
	wondererBufferFill(vbo, vertices, vertices_bytes_size);

	ibo = wondererBufferCreate(GL_ELEMENT_ARRAY_BUFFER, indices_size, GL_STATIC_DRAW);
	wondererBufferFill(ibo, indices, indices_size);

	this->layout = 
}
