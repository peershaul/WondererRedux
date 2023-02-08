#include "../../wonderer/include/engine.h"

#include "../../physc/include/body.h"

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <stdint.h>
#include <cglm/affine-pre.h>
#include <malloc.h>
#include <math.h>

#define WINDOW_WIDTH 1080
#define WINDOW_HEIGHT 1080
#define WINDOW_NAME "Wonderer | Orbiter"
#define WORLD_WIDTH 500
#define OBJECT_COUNT 2

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

void gravitational_force(float dt, Body *parent, Body *another, vec2 acceleration){
	const double G = 6.67428e-11;
	vec2 distance;

	glm_vec2_sub(another->position, parent->position, distance);
	double distance_mag = sqrt(distance[0] * distance[0] + distance[1] * distance[1]);
	
	double scaler = (another->mass * G) / pow(distance_mag, 3);
	glm_vec2_scale(distance, scaler, acceleration);
	
	WondererLog("body: %lx: (%f, %f)\n", parent, acceleration[0], acceleration[1]);	
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

int main(){
	if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, WONDERER_WINDOW_FLOATING_FLAG))
		return -1;

	float world_height = (float) WINDOW_HEIGHT / WINDOW_WIDTH * WORLD_WIDTH;

	wondererEngineInit();
	wondererCamera2DUpdateProjection(&camera, WORLD_WIDTH, world_height);

	mat4 cam_matrix;
	wondererCamera2DGetMatrix(&camera, cam_matrix);

	float startTime = glfwGetTime();
	float lastTime, dt = -1;

	struct DemoObject objects[OBJECT_COUNT] = {
		(struct DemoObject){
			.body = {1, {WORLD_WIDTH / 2.0f + 200, world_height / 2.0f}, {0, -50.167}},
			.scale = 5,
			.color = {0.6, 0.3, 0.4}
		},
		(struct DemoObject){
			.body = {1e16, {WORLD_WIDTH / 2.0f, world_height / 2.0f}, {0, 0}},
			.scale = 10,
			.color = {0.3, 0.8, 0.1} 
		}
	};

	setupObjects(objects, cam_matrix, OBJECT_COUNT);

		
	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();
		if(dt > 0){
			physcBodiesSimStep(dt);	
			updateBatch(&batch, objects);		
			wondererDrawerDrawByDataInstanced(&batch.data, batch.object_count);
		}
		lastTime = glfwGetTime();
		dt = lastTime - startTime;
		startTime = lastTime;
	}


	wondererEngineDestroy();
	free(batch.matrices);
}

