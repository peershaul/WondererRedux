#include "../../wonderer/include/engine.h"

#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_NAME "Wonderer | Batch Rendering"
#define ELEMENT_COUNT 200

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

static Camera2D camera = {};

static void generate_squares(mat4 *matrices, vec3 *colors){
	for(int i = 0; i < ELEMENT_COUNT; i++){
		glm_vec3_copy((vec3){((float)rand() / (float)(RAND_MAX)),
							 ((float)rand() / (float)(RAND_MAX)), 
							 ((float)rand() / (float)(RAND_MAX))}, colors[i]);

		float scale = ((float) rand() / (float)(RAND_MAX)) * 90 + 10;
		vec3 position = {((float) rand() / (float)(RAND_MAX)) * (WINDOW_WIDTH - scale),
						 ((float) rand() / (float)(RAND_MAX)) * (WINDOW_HEIGHT - scale), 0};
		vec3 scale_vec = {scale, scale, 1};

		mat4 model;
		glm_mat4_identity(model);

		glm_translate(model, position);
		glm_scale(model, scale_vec);

		glm_mat4_copy(model, matrices[i]);
	}
}

int main(){
	srand(time(NULL));
	if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, WONDERER_WINDOW_FLOATING_FLAG))
		return -1;

	wondererEngineInit();
	wondererCamera2DUpdateProjection(&camera, WINDOW_WIDTH, WINDOW_HEIGHT);

	mat4 cam_matrix;
	wondererCamera2DGetMatrix(&camera, cam_matrix);

	mat4 matrices[ELEMENT_COUNT];
	vec3 colors[ELEMENT_COUNT];
	
	generate_squares(matrices, colors);

	DrawData data = {
		.shader = wondererShaderGet("demos/batch_rendering/basic.vert", 
									"demos/batch_rendering/basic.frag"),
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.number_of_indices = 6,
		.texture_start_slot = 0
	};

	wondererDrawerGenerateLayout(&data, box_vertices, sizeof(box_vertices),
								box_indices, sizeof(box_indices), box_layout_sections,
								sizeof(box_layout_sections) / sizeof(uint8_t));

	char *names[] = {"cam_matrix", "matrices", "colors"};
	DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_VEC3};
	uint16_t lengths[] = {1, ELEMENT_COUNT, ELEMENT_COUNT};
	void *ptrs[] = {&cam_matrix[0][0], matrices, colors};
	
	wondererDrawerSetUniforms(&data, names, types, lengths, ptrs, 3);

	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();

		glm_mat4_identity(cam_matrix);
		wondererCamera2DGetMatrix(&camera, cam_matrix);

		wondererDrawerDrawByDataInstanced(&data, ELEMENT_COUNT);
	}

	wondererEngineDestroy();
}
