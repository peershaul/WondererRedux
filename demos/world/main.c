#include "../../wonderer/include/engine.h"
#include "../../wonderer/include/camera3D.h"

#include <GLFW/glfw3.h>

#include <cglm/cglm.h>
#include <cglm/mat4.h>

#include <cglm/util.h>
#include <cglm/vec3.h>
#include <stdint.h>
#include <stdbool.h>

#define WINDOW_HEIGHT 720 
#define WINDOW_WIDTH 1280
#define WINDOW_TITLE "Actual World of Wonderer" 
#define CAMERA_SPEED 0.1 

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
static const vec2 angle_velocities = {0.5, 0.5};
static const vec3 initial_lookat = {0, 1, 0};
static const vec3 initial_up = {0, 0, 1};

static Camera3D camera = {
	.position = {0, 0, 0},
	.look_at = {0, 1, 0},
	.up = {0, 0, 1}
};

static bool dragging = false;
static vec2 mouse_position = {};
static vec2 angles = {};
static vec3 move_direction = {};

static void glfw_mouse_click_callback(int button, int action, int mods){
	if(button == GLFW_MOUSE_BUTTON_LEFT){
		if(action == GLFW_PRESS) dragging = true;
		else if(action == GLFW_RELEASE) dragging = false;
	}
}

static void glfw_mouse_position_callback(double xpos, double ypos){
	glm_vec2_copy((vec2){xpos, ypos}, mouse_position);
}

static void glfw_key_callback(int button, int scancode, int action, int mods){
	if(action == GLFW_PRESS){
		switch(button){
			case GLFW_KEY_W: move_direction[0]++; break;
			case GLFW_KEY_S: move_direction[0]--; break;
			case GLFW_KEY_D: move_direction[1]++; break;
			case GLFW_KEY_A: move_direction[1]--; break; 
			case GLFW_KEY_R: move_direction[2]++; break;
			case GLFW_KEY_F: move_direction[2]--; break;
		}
	}
	else if(action == GLFW_RELEASE){
		switch(button){
			case GLFW_KEY_W: move_direction[0]--; break;
			case GLFW_KEY_S: move_direction[0]++; break;
			case GLFW_KEY_D: move_direction[1]--; break;
			case GLFW_KEY_A: move_direction[1]++; break; 
			case GLFW_KEY_R: move_direction[2]--; break;
			case GLFW_KEY_F: move_direction[2]++; break;
		}
	}
}

static void initialize_cube(DrawData *cube, mat4 cam_matrix, mat4 cube_model){

	*cube = (DrawData){
		.shader = wondererShaderGet("demos/world/basic.vert",
									"demos/world/basic.frag"),
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.number_of_indices = sizeof(cube_indices) / sizeof(uint8_t),
		.texture_start_slot = 0
	};

	wondererDrawerGenerateLayout(cube, cube_vertices, sizeof(cube_vertices),
								 cube_indices, sizeof(cube_indices), 
								 cube_layout_sections, 
								 sizeof(cube_layout_sections) / sizeof(uint8_t));

	const int uniform_amount = 2;
	char *uniform_names[] = {"cam_matrix", "model"};
	DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4};
	uint16_t lengths[] = {1, 1};
	void *ptrs[] = {&cam_matrix[0][0], &cube_model[0][0]};

	wondererDrawerSetUniforms(cube, uniform_names, types, lengths, ptrs, uniform_amount);
} 

static void calc_model_cube(mat4 model_matrix){
	glm_mat4_identity(model_matrix);
	glm_translate(model_matrix, (vec3){0, 100, 0});
}

static void update_camera(bool first_dragging_frame, float dt, vec2 last_mouse_pos, mat4 cam_matrix){
	if(dragging && !first_dragging_frame){
		vec2 diff;
		glm_vec2_sub(mouse_position, last_mouse_pos, diff);
		glm_vec2_scale(diff, dt, diff);
		glm_vec2_mul(diff, (float*) angle_velocities, diff);

		glm_vec2_add(angles, diff, angles);

		vec3 look_at, up;
		glm_vec3_copy((float*) initial_lookat, look_at);
		glm_vec3_copy((float*) initial_up, up);

		glm_vec3_rotate(look_at, glm_rad(angles[0]), (vec3){0, 0, 1});

		glm_vec3_rotate(look_at, glm_rad(angles[1]), (vec3){1, 0, 0});

		glm_vec3_rotate(up, glm_rad(angles[0]), (vec3){0, 0, 1});
		glm_vec3_rotate(up, glm_rad(angles[1]), (vec3){1, 0, 0});

		glm_vec3_copy(look_at, camera.look_at);
		glm_vec3_copy(up, camera.up);	
	}	

	vec3 camera_position_diff;
	glm_vec3_copy(move_direction, camera_position_diff);
	glm_vec3_scale(camera_position_diff, dt * CAMERA_SPEED, camera_position_diff);
	
	vec3 axis[3];
	glm_vec3_copy(camera.look_at, axis[0]);
	glm_vec3_copy(camera.up, axis[2]);
	glm_vec3_cross(camera.look_at, camera.up, axis[1]);

	for(int i = 0; i < 3; i++){
		vec3 displacement_component;
		glm_vec3_scale(axis[i], camera_position_diff[i], displacement_component);
		glm_vec3_add(camera.position, displacement_component, camera.position);
	}

	wondererCamera3DGetMatrix(&camera, cam_matrix);
}

int main(){
	if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, WONDERER_WINDOW_FLOATING_FLAG))
		return -1;

	wondererEngineInit();

	wondererWindowAddCallback(WONDERER_WINDOW_MOUSE_PRESS_CALLBACK, glfw_mouse_click_callback);
	wondererWindowAddCallback(WONDERER_WINDOW_CURSOR_POS_CALLBACK, glfw_mouse_position_callback);
	wondererWindowAddCallback(WONDERER_WINDOW_KEY_CALLBACK, glfw_key_callback);

	wondererCamera3DUpdateProjection(&camera, glm_rad(60), 
			(float) WINDOW_WIDTH / WINDOW_HEIGHT, 0.1, 1000);

	mat4 cube_model, cam_matrix;
	wondererCamera3DGetMatrix(&camera, cam_matrix);

	DrawData cube_data;
	initialize_cube(&cube_data, cam_matrix, cube_model);
	calc_model_cube(cube_model);

	bool dragging_last_frame = false;
	vec2 last_mouse_pos = {};

	float startTime = glfwGetTime();
	float dt = -1, endTime;

	vec2 cube_angle = {};
	const vec2 cube_angular_velocity = {20, 10};

	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();
		
		if(dt > 0){
			update_camera(dragging && !dragging_last_frame, dt, last_mouse_pos, cam_matrix);
	
			if(dragging != dragging_last_frame) dragging_last_frame = !dragging_last_frame;
			if(dragging) glm_vec2_copy(mouse_position, last_mouse_pos);  

#if 0
			WondererLog("Camera Position: (%f, %f, %f)\nLook at: (%f, %f, %f)\nUp: (%f, %f, %f)",
						camera.position[0], camera.position[1], camera.position[2], camera.look_at[0],
						camera.look_at[1], camera.look_at[2], camera.up[0], camera.up[1], camera.up[2]);
#endif
			vec2 slot;
			glm_vec2_scale((float*) cube_angular_velocity, dt, slot);
			glm_vec2_add(cube_angle, slot, cube_angle);

			glm_mat4_identity(cube_model);

			glm_translate(cube_model, (vec3){0, 50, 0});
			glm_rotate(cube_model, glm_rad(cube_angle[0]), (vec3){0, 0, 1});
			glm_rotate(cube_model, glm_rad(cube_angle[1]), (vec3){1, 0, 0}); 
			glm_scale(cube_model, (vec3){0.2, 0.2, 0.2});

			wondererDrawerDrawByData(&cube_data);
		}

		endTime = glfwGetTime();
		dt = endTime - startTime;
		startTime = endTime;
	}

	wondererEngineDestroy();
}

