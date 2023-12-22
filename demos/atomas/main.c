#include "../../wonderer/include/engine.h"

#include <stdio.h>
#include <stdint.h>

#include <cglm/cglm.h>

#define INI_WIDTH 1280
#define INI_HEIGHT 720
#define INI_TITLE "Wonderer | Atomas"

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

static void initialize_circle(DrawData *circle, mat4 cam_matrix, mat4 model){
    *circle = (DrawData){
        .shader = wondererShaderGet("demos/atomas/circle.vert",
                                    "demos/atomas/circle.frag"),
        .number_of_indices = sizeof(box_indices) / sizeof(uint8_t), 
        .data_type_of_indices = GL_UNSIGNED_BYTE,
        .texture_start_slot = 0
    };

    wondererDrawerGenerateLayout(circle, box_vertices, sizeof(box_vertices),
                                 box_indices, sizeof(box_indices), 
                                 box_layout_sections, sizeof(box_layout_sections));

    glm_mat4_identity(model);

    glm_translate(model, (vec3){100, 100, 1});
    glm_scale(model, (vec3){200, 200, 1});

    char *uniform_names[] = {"cam_matrix", "model"};
    DrawDataType uniform_types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4};
    void *uniform_ptrs[] = {&cam_matrix[0][0], &model[0][0]};
    uint16_t lengths[] = {1, 1};

    wondererDrawerSetUniforms(circle, uniform_names, uniform_types, lengths,
                              uniform_ptrs, 2);    

}

int main(){
    if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE, \
                           WONDERER_WINDOW_FLOATING_FLAG))
        exit(-1);

    wondererEngineInit();

    Camera2D camera = {};
    wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);

    mat4 cam_matrix, model;
    wondererCamera2DGetMatrix(&camera, cam_matrix);

    DrawData circle;
    initialize_circle(&circle, cam_matrix, model);

    while(!wondererWindowShouldClose()){
        wondererWindowUpdate();
        wondererDrawerDrawByData(&circle);
    }

    wondererEngineDestroy();
    wondererDestroyWindow();
}