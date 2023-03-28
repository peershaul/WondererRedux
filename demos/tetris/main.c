#include "../../wonderer/include/engine.h" 

#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <stdint.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 1000
#define WINDOW_TITLE "Wonderer Multi | Tetris" 

#define TETRIS_GRID_WIDTH 10
#define TETRIS_GRID_HEIGHT 20 

// Tetris grid is 20x10

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


static void calc_squares(mat4 *models){
	const int squares_combined_width = 450;
	const int squares_padding = 1;
	// Grid is centered in the y axis
	
	const int square_size = (squares_combined_width - 11 * squares_padding) / (2 * TETRIS_GRID_WIDTH);
	const int initial_y_position = (WINDOW_HEIGHT / 2.0f) - (TETRIS_GRID_HEIGHT / 2.0f) * (2 * square_size + squares_padding);
	const int initial_x_position = (WINDOW_WIDTH - squares_combined_width) / 2;

	for(int y = 0; y < TETRIS_GRID_HEIGHT; y++){
		int ypos = initial_y_position + square_size * (1 + 2 * y) + squares_padding * y;
		for(int x = 0; x < TETRIS_GRID_WIDTH; x++){
			int xpos = initial_x_position + square_size * (1 + 2 * x) + squares_padding * (1 + x);
			vec3 position = {xpos, ypos, 0};

			mat4 model;
			glm_mat4_identity(model);

			glm_translate(model, position);
			glm_scale(model, (vec3){square_size, square_size, 1});

			glm_mat4_identity(models[TETRIS_GRID_WIDTH * y + x]);
			glm_mat4_copy(model, models[TETRIS_GRID_WIDTH * y + x]);
		}
	}
}

static void generateDrawData(DrawData *data, mat4 *models, mat4 cam_matrix){
	*data = (DrawData){
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.shader = wondererShaderGet("demos/tetris/piece.vert",
									"demos/tetris/piece.frag"),
		.number_of_indices = sizeof(box_indices) / sizeof(uint8_t),
		.texture_start_slot = 0
	};

	wondererDrawerGenerateLayout(data, box_vertices, sizeof(box_vertices), 
								 box_indices, sizeof(box_indices), box_layout_sections, 
								 sizeof(box_layout_sections) / sizeof(uint8_t));
	
	char *names[] = {"cam_matrix", "models"};
	uint16_t lengths[] = {1, 200};
	DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4};
	void *ptrs[] = {&cam_matrix[0][0], models};

	wondererDrawerSetUniforms(data, names, types, lengths, ptrs, 2);
}

int main(){
	if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, WONDERER_WINDOW_FLOATING_FLAG))
		return -1;
	
	wondererEngineInit();

	mat4 cam_matrix;
	wondererCamera2DUpdateProjection(&camera, WINDOW_WIDTH, WINDOW_HEIGHT);
	wondererCamera2DGetMatrix(&camera, cam_matrix);

	mat4 square_models[TETRIS_GRID_WIDTH * TETRIS_GRID_HEIGHT];
	calc_squares(square_models);

	DrawData draw_data;
	generateDrawData(&draw_data, square_models, cam_matrix);

	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();
		wondererDrawerDrawByDataInstanced(&draw_data, 200);
	}

	wondererEngineDestroy();
	
}
