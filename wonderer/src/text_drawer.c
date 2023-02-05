#include "../../wonderer/include/text_drawer.h"

#include <string.h>

static float box_vertices[] = {
	-1, -1, 
	-1,  1, 
	 1,  1, 
	 1, -1
};

static uint8_t box_indices[] = {
	1, 0, 2, 
	0, 3, 2
}; 

static uint8_t box_layout_sections[] = {2};

void wondererGenerateTextBox(TextBox *textbox, const char *text, vec4 *cam_matrix,
							 float *position, Shader *shader, uint16_t line_height,								uint16_t width)
{
	uint16_t length = strlen(text); 
	textbox->length = length;
	textbox->line_height = line_height;
	textbox->width = width;
	if(textbox->threshold == 0) textbox->threshold = 0.5f;
	
	textbox->data = (DrawData){
		.shader = shader,
		.texture_start_slot = 0,
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.number_of_indices = sizeof(box_indices) / sizeof(uint8_t),
	};
	
	wondererDynamicArrayInitialize(textbox->length, sizeof(int32_t),
								   &textbox->letters);
	wondererDynamicArrayInitialize(textbox->length, sizeof(mat4), 
								   &textbox->models);
	float initial_xpos = position[0] - width * 0.5; 
	vec2 curr_position = {initial_xpos, position[1]};

	int32_t *letters = textbox->letters.arr;
	mat4 *models = textbox->models.arr;

	for(int i = 0; i < length; i++){
		bool is_new_line = text[i] == '\n';
		if(!is_new_line){
			letters[textbox->letters.last++] = text[i] - 32;
			vec3 curr_position = {curr_position[0], curr_position[1], 0};
			mat4 model;
			glm_mat4_identity(model);
			glm_translate(model, curr_position);
			glm_scale(model, (vec3){line_height * 0.5, line_height * 0.5, 1});
			glm_mat4_copy(model, models[textbox->models.last++]);
			curr_position[0] += line_height;
		}
		else textbox->length--;
		if((curr_position[0] > position[0] + width * 0.5) || is_new_line){
			curr_position[0] = initial_xpos;
			curr_position[1] += line_height;
		}
	} 

	wondererDrawerGenerateLayout(&textbox->data, box_vertices, 
								 sizeof(box_vertices), box_indices, 
								 sizeof(box_indices), box_layout_sections, 
								 sizeof(box_layout_sections) / sizeof(uint8_t));

	char *uniform_names[6] = {"cam_matrix", "models", "letters", "font", "cols", 
						      "threshold"};
	DrawDataType types[6] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4, 
							 WONDERER_DRAW_DATA_INT,  WONDERER_DRAW_DATA_TEXTURE,
							 WONDERER_DRAW_DATA_INT,  WONDERER_DRAW_DATA_FLOAT};
	uint16_t lengths[6] = {1, textbox->length, textbox->length, 1, 1, 1};
	void *ptrs[6] = {&cam_matrix[0][0], textbox->models.arr, textbox->letters.arr,
					 NULL, &textbox->font.cols, &textbox->threshold};

	wondererDrawerSetUniforms(&textbox->data, uniform_names, types, lengths, ptrs,
							  6);
	wondererDrawerUploadTextures(&textbox->data, &textbox->font.texture, 1);
}

