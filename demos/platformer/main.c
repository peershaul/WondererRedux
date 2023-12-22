#include "../../wonderer/include/engine.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <time.h>

#include <cglm/cglm.h>
	
#include "imgui.h"

#define INI_WIDTH 1280
#define INI_HEIGHT 720
#define INI_TITLE "Wonderer Multi | Platformer"

#define PLATFORM_LEVEL_LAYOUT_VERTICAL_AMOUNT 40
#define PLATFORM_LEVEL_LAYOUT_MAX_HORIZONAL_AMOUNT 5
#define PLATFORM_SCALE 20

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
static bool show_inspector = true; 

static uint16_t initialize_platforms(DrawData* data, dynamic_arr* platform_arrays, Texture **textures){

	uint8_t platform_level_layout[PLATFORM_LEVEL_LAYOUT_VERTICAL_AMOUNT];
	uint16_t platforms_total_amount = 0;
	for(uint8_t i = 0; i < PLATFORM_LEVEL_LAYOUT_VERTICAL_AMOUNT; i++){
		platform_level_layout[i] = rand() % PLATFORM_LEVEL_LAYOUT_MAX_HORIZONAL_AMOUNT;
		platforms_total_amount += platform_level_layout[i];
		if(platforms_total_amount > 100)
			WondererError("Platform amount is too large");

		printf("%d ", platform_level_layout[i]);
	}

	wondererDynamicArrayInitialize(platforms_total_amount, sizeof(mat4), &platform_arrays[0]);
	wondererDynamicArrayInitialize(platforms_total_amount, sizeof(int32_t), &platform_arrays[1]);

	uint8_t platform_index = 0;
	mat4 *model_matrices_arr = platform_arrays[0].arr;
	uint32_t *states_arr = platform_arrays[1].arr;

	for(uint8_t x = 0; x < PLATFORM_LEVEL_LAYOUT_VERTICAL_AMOUNT; x++){
		for(uint8_t y = 0; y < platform_level_layout[x]; y++){
			vec3 curr_platform_position = {2 * x + 1, 2 * y + 1, 0};
			int32_t curr_state = y == platform_level_layout[x] - 1? 1 : 0;

			mat4 model_matrix;
			glm_mat4_identity(model_matrix);

			glm_scale(model_matrix, (vec3){PLATFORM_SCALE, PLATFORM_SCALE, 1});
			glm_translate(model_matrix, curr_platform_position);

			glm_mat4_copy(model_matrix, model_matrices_arr[platform_index]);
			states_arr[platform_index] = curr_state; 

			printf("%d ", curr_state);

			platform_index++;
		}
		printf("\n");
	}

	Shader *shader = wondererShaderGet("demos/platformer/resources/platforms.vert", \
																		 "demos/platformer/resources/platforms.frag");
	char *texture_locations[] = {"demos/platformer/resources/GrassBlock.png", \
															 "demos/platformer/resources/DirtBlock.png"};

	for(uint8_t i = 0; i < 2; i++){
		textures[i] = wondererTextureCreate();
		wondererTextureUploadTexImage(textures[i], texture_locations[i]);
	}

	*data = (DrawData){
		.shader = shader,
		.data_type_of_indices = GL_UNSIGNED_BYTE, 
		.texture_start_slot = 0,
		.number_of_indices = sizeof(box_indices) / sizeof(uint8_t)
	};

	wondererDrawerGenerateLayout(data, box_vertices, sizeof(box_vertices), \
															box_indices, sizeof(box_indices), \
															box_layout_sections, sizeof(box_layout_sections));

	#define uniform_amount 5
	char *uniforms_names[uniform_amount] = {"model_matrices", "is_tops", "cam_matrix", "tex0", "tex1"};			
	DrawDataType types[uniform_amount] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_INT, \
																				WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_TEXTURE, \
																				WONDERER_DRAW_DATA_TEXTURE};

	void *ptrs[uniform_amount] = {model_matrices_arr, states_arr, NULL, NULL, NULL};
	uint16_t lengths[uniform_amount] = {platforms_total_amount, platforms_total_amount, 1, 1, 1};					
	
	wondererDrawerSetUniforms(data, uniforms_names, types, lengths, ptrs, uniform_amount);
	wondererDrawerUploadTextures(data, textures, 2);
	
	return platforms_total_amount;
}

int main(){
	srand(time(NULL));

	if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE, \
			WONDERER_WINDOW_FLOATING_FLAG))
		return -1;

	wondererEngineInit();
	wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);

	DrawData platforms_data = {};
	dynamic_arr platforms_arrays[2];
	Texture *textures[2];
	uint16_t platfroms_count = initialize_platforms(&platforms_data, platforms_arrays, textures);	

	float startTime = glfwGetTime();
	float dt = -1, lastTime;

	bool reload_platforms = false;

	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();
		wondererImguiNewFrame();

		mat4 cam_matrix;
		wondererCamera2DGetMatrix(&camera, cam_matrix);

		imguiInspector(dt, &show_inspector, &reload_platforms);	

		if(reload_platforms){
			reload_platforms = false;
			wondererDrawerDestroyDrawData(&platforms_data);
			platfroms_count = initialize_platforms(&platforms_data, platforms_arrays, textures);	
		}

		void** platforms_ptrs = platforms_data.data_ptrs.arr;
		platforms_ptrs[2] = &cam_matrix[0][0];

		for(uint8_t i = 0; i < 2; i++)
			wondererTextureBind(i, textures[i]);

		wondererDrawerDrawByDataInstanced(&platforms_data, platfroms_count);

		wondererImguiDraw();

		lastTime = glfwGetTime();
		dt = lastTime - startTime;
		startTime = lastTime;
	}

	free(platforms_arrays[0].arr);
	free(platforms_arrays[1].arr);
	wondererDrawerDestroyDrawData(&platforms_data);
	wondererEngineDestroy();
	wondererDestroyWindow();
}
