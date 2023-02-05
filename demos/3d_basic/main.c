#include "../../wonderer/include/logger.h"
#include "../../wonderer/include/window.h"
#include "../../wonderer/include/imgui_cmpl.h"

#include "../../wonderer/include/shader.h"
#include "../../wonderer/include/layout.h"
#include "../../wonderer/include/buffer.h"
#include "../../wonderer/include/drawer.h"

#include "../../wonderer/include/camera3D.h"

#include "3d_basic_imgui.h"

#include <cglm/vec3.h>
#include <stdbool.h>
#include <math.h>

#define INI_WIDTH 1280
#define INI_HEIGHT 720 
#define INI_TITLE "3D MAGIC"

static float pyramid_verts[] = {
	// Position             Color
	// Bottom Square
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f, 
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 
	// Front Triangle 
		 1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f, 
		 0.0f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f,
	
	// Left Triangle
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		 0.0f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f,

	// Back Triangle
	  -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		 0.0f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 

	// Right Triangle 
	   1.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
		-1.0f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f, 
		 0.0f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f
};

static float square_verts[] = {
	// Bottom face
	-1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
	-1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 
	 1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,

	// Up face
	-1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 
	-1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f,
	 1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 
	 1.0f, -1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 

	// Front Face
   1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 
	-1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
	-1.0f,  1.0f, -1.0f,  0.0f, 1.0f, 0.0f,

	// Back Face 
	 1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f, 
	 1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 
	-1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 
	-1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 0.0f,
	
	// Left Face 
	-1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 
	-1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 
	-1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	-1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,

	// Right Face 
	 1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f, 
	 1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f, 
	 1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
	 1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f

};

static uint8_t pyramid_indices[] = {
	1, 0, 2, 
	0, 3, 2,

	4, 5, 6,
	7, 8, 9,

	10, 11, 12, 
	13, 14, 15,
};

static uint8_t square_indices[]={
	1, 0, 2, 
	0, 3, 2,

	5, 4, 6, 
	4, 7, 6, 

	9, 8, 10, 
	8, 11, 10,

	13, 12, 14, 
	12, 15, 14,

	17, 16, 18,
	16, 19, 18,

	21, 20, 22, 
	20, 23, 22
};

static uint8_t pyramid_layout_sections[] = {3, 3};

static Camera3D camera = {};
static int8_t camera_move_direction[3] = {};

static void key_callback(int key, int scancode, int action, int mods){
	if(action == GLFW_PRESS){
		switch(key){
			case GLFW_KEY_LEFT_SHIFT:
				camera_move_direction[0]++;
				return;
			case GLFW_KEY_LEFT_CONTROL:
				camera_move_direction[0]--;
				return;

			case GLFW_KEY_W:
				camera_move_direction[1]++;
				return;
			case GLFW_KEY_S:
				camera_move_direction[1]--;
				return;

			case GLFW_KEY_D:
				camera_move_direction[2]++;
				return;
			case GLFW_KEY_A:
				camera_move_direction[2]--;
				return;
		}
	}

	if(action == GLFW_RELEASE){
		switch(key){
			case GLFW_KEY_LEFT_SHIFT:
				camera_move_direction[0]--;
				return;
			case GLFW_KEY_LEFT_CONTROL:
				camera_move_direction[0]++;
				return;

			case GLFW_KEY_W:
				camera_move_direction[1]--;
				return;
			case GLFW_KEY_S:
				camera_move_direction[1]++;
				return;

			case GLFW_KEY_D:
				camera_move_direction[2]--;
				return;
			case GLFW_KEY_A:
				camera_move_direction[2]++;
				return;
		}
	}
}

static void engine_init(){

	wondererShaderLoaderInit();
	wondererLayoutLoaderInit();
	wondererBufferLoaderInit();

	wondererWindowAddCallback(WONDERER_WINDOW_KEY_CALLBACK, key_callback);

	wondererImguiInit();
}

static void engine_destroy(){

	wondererShaderLoaderTerminate();
	wondererBufferLoaderTerminate();
	wondererLayoutLoaderTerminate();

	wondererImguiDestroy();
}

static void prepare_pyramid(DrawData *data, mat4 trans_matrix){
	Buffer *ibo, *vbo;
	Layout *layout;

	vbo = wondererBufferCreate(GL_ARRAY_BUFFER, sizeof(pyramid_verts), GL_STATIC_DRAW);
	wondererBufferFill(vbo, pyramid_verts, sizeof(pyramid_verts));
	
	ibo = wondererBufferCreate(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyramid_indices), GL_STATIC_DRAW);
	wondererBufferFill(ibo, pyramid_indices, sizeof(pyramid_indices));

	layout = wondererLayoutCreate(pyramid_layout_sections, 
			sizeof(pyramid_layout_sections) / sizeof(uint8_t), vbo, ibo);	

	*data= (DrawData){
		.shader = wondererShaderGet("demos/3d_basic/pyramid.vert", "demos/3d_basic/pyramid.frag"),
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.number_of_indices = sizeof(pyramid_indices) / sizeof(uint8_t),
		.layout = layout
	};

	wondererDrawerDrawDataInitializeLength(data, 1);

	int *uniforms = data->uniforms.arr;
	DrawDataType *types = data->types.arr;
	uint8_t *lengths = data->lengths.arr;
	void **data_ptrs = data->data_ptrs.arr;
	
	uniforms[0] = glGetUniformLocation(wondererShaderGetID(data->shader), "trans_matrix");
	types[0] = WONDERER_DRAW_DATA_MAT4;
	lengths[0] = 1;
	data_ptrs[0] = &trans_matrix[0][0];

	wondererDrawerDrawDataCommitLength(data, 1);
} 

static void prepare_square(DrawData *data, mat4 trans_matrix){
	Buffer *ibo, *vbo;
	Layout *layout;

	vbo = wondererBufferCreate(GL_ARRAY_BUFFER, sizeof(square_verts), GL_STATIC_DRAW);
	wondererBufferFill(vbo, square_verts, sizeof(square_verts));
	
	ibo = wondererBufferCreate(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), GL_STATIC_DRAW);
	wondererBufferFill(ibo, square_indices, sizeof(square_indices));

	layout = wondererLayoutCreate(pyramid_layout_sections, 
			sizeof(pyramid_layout_sections) / sizeof(uint8_t), vbo, ibo);	

	*data= (DrawData){
		.shader = wondererShaderGet("demos/3d_basic/pyramid.vert", "demos/3d_basic/pyramid.frag"),
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.number_of_indices = sizeof(square_indices) / sizeof(uint8_t),
		.layout = layout
	};

	wondererDrawerDrawDataInitializeLength(data, 1);

	int *uniforms = data->uniforms.arr;
	DrawDataType *types = data->types.arr;
	uint8_t *lengths = data->lengths.arr;
	void **data_ptrs = data->data_ptrs.arr;
	
	uniforms[0] = glGetUniformLocation(wondererShaderGetID(data->shader), "trans_matrix");
	types[0] = WONDERER_DRAW_DATA_MAT4;
	lengths[0] = 1;
	data_ptrs[0] = &trans_matrix[0][0];

	wondererDrawerDrawDataCommitLength(data, 1);
} 

void calculate_camera_position(float dt){
	vec3 look_direction, look_distance;

	const float axial_velocity = 10;

	glm_vec3_sub(camera.position, camera.look_at, look_distance);
	glm_vec3_normalize_to(look_distance, look_direction);
	
	vec3 cam_right;
	glm_cross(camera.up, look_direction, cam_right);
	glm_vec3_normalize(cam_right);

	float look_radius = glm_vec3_norm(look_distance);
	float updated_look_radius = look_radius + dt * axial_velocity * camera_move_direction[0];
	
	vec3 new_position;
	glm_vec3_copy(camera.position, new_position);
	
	float delta_right = dt * axial_velocity * camera_move_direction[2];
	float delta_up = dt * axial_velocity * camera_move_direction[1];
	
	vec3 vdelta_right, vdelta_up;

	glm_vec3_scale(cam_right, delta_right, vdelta_right);
	glm_vec3_add(new_position, vdelta_right, new_position);

	vec3 new_distance;
	glm_vec3_sub(new_position, camera.look_at, new_distance);
	glm_cross(camera.up, new_distance, cam_right);
	glm_vec3_normalize(cam_right);

	glm_vec3_scale(camera.up, delta_up, vdelta_up);
	glm_vec3_add(new_position, vdelta_up, new_position);
	
	glm_vec3_sub(new_position, camera.look_at, new_distance);
	glm_cross(new_distance, cam_right, camera.up);
	glm_vec3_normalize(camera.up);

	vec3 new_direction;
	glm_vec3_sub(new_position, camera.look_at, new_distance);
	glm_vec3_normalize_to(new_distance, new_direction);

	glm_vec3_scale(new_direction, updated_look_radius, new_position);

	glm_vec3_copy(new_position, camera.position);
}

int main(){

	WondererLog("Lets program in 3D");

	if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE, 
				WONDERER_WINDOW_FLOATING_FLAG))
		return -1;	

	bool show_inspector = true;

	engine_init();

	glm_vec3_copy((vec3){0, 0, 0}, camera.look_at);
	glm_vec3_copy((vec3){0, 7, 4}, camera.position);
	glm_vec3_copy((vec3){0, 0, 1}, camera.up);	

	WondererLog("Camera Up: (%f, %f, %f)", camera.up[0], camera.up[1], camera.up[2]);

	mat4 pyramid_trans_matrix, square_trans_matrix;
	DrawData pyramid_data, square_data;

	prepare_pyramid(&pyramid_data, pyramid_trans_matrix);
	prepare_square(&square_data, square_trans_matrix);

	wondererCamera3DUpdateProjection(&camera, 60, (float) INI_WIDTH / INI_HEIGHT, 0.1, 100);

	float startTime = glfwGetTime(), dt = -1, lastTime;
	float angle = 0.0f;
	float radial_speed = 0;

	while(!wondererWindowShouldClose()){
		wondererImguiNewFrame();

		calculate_camera_position(dt);

		imgui_inspector_window(&show_inspector, &camera, &radial_speed, dt);
		
		
		mat4 cam_matrix, model_matrix;
		wondererCamera3DGetMatrix(&camera, cam_matrix);
		if(dt > 0)
			angle += dt * radial_speed;

		glm_mat4_identity(model_matrix);
		glm_translate(model_matrix, (vec3){-2.0f, 0.0f, -1.0f});
		glm_rotate_z(model_matrix, angle, model_matrix);
		glm_scale(model_matrix, (vec3){1, 1, 1.414});

		glm_mat4_mul(cam_matrix, model_matrix, pyramid_trans_matrix);

		wondererDrawerDrawByData(&pyramid_data);

		glm_mat4_identity(model_matrix);
		glm_translate(model_matrix, (vec3){2.0f, 0.0f, 0.0f});
		glm_rotate_z(model_matrix, angle, model_matrix);

		glm_mat4_mul(cam_matrix, model_matrix, square_trans_matrix);

		wondererDrawerDrawByData(&square_data);

		wondererImguiDraw();
		wondererWindowUpdate();

		lastTime = glfwGetTime();
		dt = lastTime - startTime;
		startTime = lastTime;
	}

	wondererDrawerDestroyDrawData(&pyramid_data);
	engine_destroy();
	wondererDestroyWindow();
}
