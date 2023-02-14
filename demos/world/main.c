#include "../../wonderer/include/engine.h"
#include "../../wonderer/include/camera3D.h"
#include <GLFW/glfw3.h>
#include <cglm/mat4.h>

#define WINDOW_HEIGHT 720
#define WINDOW_WIDTH 1280
#define WINDOW_TITLE "Actual World of Wonderer"

static float cube_vertices[] = {
	// front face
    -1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
    
    // back face
    -1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    
    // left face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    
    // right face
     1.0f, -1.0f, -1.0f,  1.0f,  1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  0.0f,
    
    // top face
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    
    // bottom face
    -1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  1.0f
};

static uint8_t cube_indices[] = {
	// front face
    0, 1, 2,
    2, 3, 0,
    
    // back face
    4, 5, 6,
    6, 7, 4,
    
    // left face
    8, 9, 10,
    10, 11, 8,
    
    // right face
    12, 13, 14,
    14, 15, 12,
    
    // top face
    16, 17, 18,
    18, 19, 16,
    
    // bottom face
    20, 21, 22,
    22, 23, 20
};

static uint8_t cube_layout_sections[] = {3, 3};

static Camera3D camera = {
	.position = {0, 0, 0},
	.look_at = {0, 0, 1},
	.up = {0, 1, 0}
};

int main(){
	if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, WONDERER_WINDOW_FLOATING_FLAG))
		return -1;

	WondererLog("Hello world from the world demo");
	wondererEngineInit();
	
	wondererCamera3DUpdateProjection(&camera, glm_rad(60), 
									(float) WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 1000);

	mat4 cam_matrix, model_matrix;

	wondererCamera3DGetMatrix(&camera, cam_matrix);

	DrawData data = {
		.shader = wondererShaderGet("demos/world/basic.vert",
									"demos/world/basic.frag"),
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.number_of_indices = sizeof(cube_indices) / sizeof(uint8_t),
		.texture_start_slot = 0,
	};
	wondererDrawerGenerateLayout(&data, cube_vertices, sizeof(cube_vertices), 
								 cube_indices, sizeof(cube_indices), 
								 cube_layout_sections, 
								 sizeof(cube_layout_sections) / sizeof(uint8_t));

	char *names[] = {"cam_matrix", "model"};
	DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4};
	uint16_t lengths[] = {1, 1};
	void *data_ptrs[] = {&cam_matrix[0][0], &model_matrix[0][0]};

	wondererDrawerSetUniforms(&data, names, types, lengths, data_ptrs, 2);
	
	float startTime = glfwGetTime();
	float dt = -1.0f, lastTime;

	vec2 angular_velocity = {0.5, 1/3.0f};
	vec2 curr_angle = {0, 0};

	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();	

		if(dt > 0){

			vec2 slot;
			glm_vec2_scale(angular_velocity, dt, slot);
			glm_vec2_add(curr_angle, slot, curr_angle);
			
			glm_mat4_identity(model_matrix);
			glm_translate(model_matrix, (vec3){0, 0, 5});
			glm_rotate(model_matrix, curr_angle[0], camera.up);
			glm_rotate(model_matrix, curr_angle[1], camera.look_at);
			glm_scale(model_matrix, (vec3){0.02, 0.02, 0.02}); 

			wondererDrawerDrawByData(&data);
		}

		lastTime = glfwGetTime();
		dt = lastTime - startTime;
		startTime = lastTime;
	}

	wondererEngineDestroy();
}
