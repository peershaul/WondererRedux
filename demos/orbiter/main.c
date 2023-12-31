#include "../../wonderer/include/engine.h"

#include "../../physc/include/body.h"

#include "orbiter.h"

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdint.h>
#include <stdbool.h>
#include <cglm/affine-pre.h>
#include <malloc.h>
#include <math.h>

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define WINDOW_NAME "Wonderer | Orbiter"
#define OBJECT_COUNT 3

struct DrawBatch {
	DrawData data;
	mat4 *matrices;
	vec3 *colors;
	uint8_t object_count;
};

struct DemoObject {
	Body body;
	float scale;
	vec3 color;
};


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
static struct DrawBatch batch = {};
static bool show_window = true;

static void glfw_key_callback(int key, int scancode, int action, int mods){
	if(key == GLFW_KEY_I && action == GLFW_PRESS)
		show_window = !show_window;
} 

void gravitational_force(float dt, Body *parent, Body *another, vec2 acceleration){
	const double G = 6.67428e-11;
	vec2 distance;

	glm_vec2_sub(another->position, parent->position, distance);
	double distance_mag = sqrt(distance[0] * distance[0] + distance[1] * distance[1]);
	
	double scaler = (another->mass * G) / pow(distance_mag, 3);
	glm_vec2_scale(distance, scaler, acceleration);
}

static void setupObjects(struct DemoObject *objects, mat4 cam_matrix, uint8_t count){	
	batch = (struct DrawBatch){
		.data = (DrawData){
			.shader = wondererShaderGet("demos/orbiter/object.vert", 
									    "demos/orbiter/object.frag"),
			.number_of_indices = 6,
			.texture_start_slot = 0,
			.data_type_of_indices = GL_UNSIGNED_BYTE,
		},
		.matrices = malloc(count * sizeof(mat4)),
		.colors = malloc(count * sizeof(vec3)),
		.object_count = count
	};

	Force forces[] = {gravitational_force};
	for(int i = 0; i < count; i++){
		physcBodyAddToSim(&objects[i].body);
		glm_vec3_copy(objects[i].color, batch.colors[i]);
		physcBodySetForces(&objects[i].body, forces, 1);
	}

	wondererDrawerGenerateLayout(&batch.data, box_vertices, sizeof(box_vertices), 
								 box_indices, sizeof(box_indices), box_layout_sections,
								 sizeof(box_layout_sections) / sizeof(uint8_t));
	
	char *uniforms[] = {"cam_matrix", "matrices", "colors"};
	DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_VEC3};
	uint16_t lengths[] = {1, count, count};
	void *ptrs[] = {cam_matrix, batch.matrices, batch.colors};
	wondererDrawerSetUniforms(&batch.data, uniforms, types, lengths, ptrs, 3);
}

static void updateBatch(struct DrawBatch *batch, struct DemoObject *object){
	for(int i = 0; i < batch->object_count; i++){
		mat4 model; 
		vec3 scale = {object[i].scale, object[i].scale, 1};
		vec3 position = {object[i].body.position[0], object[i].body.position[1], 0};

		glm_mat4_identity(model);
		glm_translate(model, position);
		glm_scale(model, scale);

		glm_mat4_copy(model, batch->matrices[i]);
	}
}

static float calc_world_width(float world_height){
	float world_width = (float) WINDOW_WIDTH / WINDOW_HEIGHT * world_height;
	wondererCamera2DUpdateProjection(&camera, world_width, world_height);
	return world_width;
}

int main(){
	if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, WONDERER_WINDOW_FLOATING_FLAG))
		return -1;
	
	float world_height = 500;
	float world_width = calc_world_width(world_height); 
	float prev_world_height = world_height;
	vec2 prev_COM = {};

	wondererEngineInit();
	wondererWindowAddCallback(WONDERER_WINDOW_KEY_CALLBACK, glfw_key_callback);

	mat4 cam_matrix;
	wondererCamera2DGetMatrix(&camera, cam_matrix);

	float startTime = glfwGetTime();
	float lastTime, dt = -1;

	const float altitude = 200;
	const float parent_mass = 1e17;
	const float orbital_velocity = sqrt(6.67e-11 * parent_mass / altitude); 

	struct DemoObject objects[OBJECT_COUNT] = {
		(struct DemoObject){
			.body = {1e16, {altitude, 0}, {0, -orbital_velocity}},
			.scale = 10,
			.color = {0.6, 0.3, 0.4}
		},
		(struct DemoObject){
			.body = {parent_mass, {0, 0}, {0, 0}},
			.scale = 10,
			.color = {0.3, 0.8, 0.1} 
		}, 
		(struct DemoObject){
			.body = {1e16, {-altitude, 0}, {0, orbital_velocity / 3.0f}},
			.scale = 5,
			.color = {0.8431, 0.6, 0.129}  
		}
	};

	setupObjects(objects, cam_matrix, OBJECT_COUNT);

		
	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();
		wondererImguiNewFrame();
	
		if(world_height != prev_world_height){
			world_width = calc_world_width(world_height);
			prev_world_height = world_height;
		}

		vec2 COM, COM_velocity, cam_position, body_positions[2];

		if(dt > 0){
			physcBodiesSimStep(dt);	
			updateBatch(&batch, objects);		
	
			glm_vec2_copy((vec2){0, 0}, COM);
			float total_mass = 0;

			for(int i = 0; i < OBJECT_COUNT; i++){
				Body *cb = &objects[i].body;
				vec2 slot;
				total_mass += cb->mass;

				glm_vec2_scale(cb->position, cb->mass, slot);
				glm_vec2_add(COM, slot, COM);
			}
			glm_vec2_scale(COM, 1.0f / total_mass, COM);

			glm_vec2_copy((vec2) {COM[0] - (world_width * 0.5f), 
					              COM[1] - (world_height * 0.5f)}, cam_position);

			glm_vec2_sub(COM, prev_COM, COM_velocity);

			for(int i = 0; i < OBJECT_COUNT; i++)
				glm_vec2_copy(objects[i].body.position, body_positions[i]);

			glm_vec2_copy(cam_position, camera.position);
			wondererCamera2DGetMatrix(&camera, cam_matrix);
			
			wondererDrawerDrawByDataInstanced(&batch.data, batch.object_count);
		}

		Inspector(dt, &show_window, &world_height, COM, COM_velocity, 
			      cam_position, body_positions, OBJECT_COUNT);
		wondererImguiDraw();
		
		glm_vec2_copy(COM, prev_COM);

		lastTime = glfwGetTime();
		dt = lastTime - startTime;
		startTime = lastTime;
	}


	wondererEngineDestroy();
	free(batch.matrices);
}

