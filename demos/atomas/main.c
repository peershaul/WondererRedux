#include "../../wonderer/include/engine.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include <cglm/cglm.h>

#define INI_WIDTH 1280
#define INI_HEIGHT 720
#define INI_TITLE "Wonderer | Atomas"

#define PI 3.14159f

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

static void initialize_circle(DrawData *circle, mat4 cam_matrix, mat4 *models, const int num_circles){
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

    float angle_increment = 2 * PI / num_circles;

    int half_width = INI_WIDTH / 2;
    int half_height = INI_HEIGHT / 2;

    int displacement_radius = 300;
    int circle_radius = 50;

    // y axis => cos, x axis => sin
    for(int i = 0; i < num_circles; i++){
        mat4 model;
        glm_mat4_identity(model);

        vec3 position =  {round(half_width  + displacement_radius * sin(angle_increment * i)),
                          round(half_height + displacement_radius * cos(angle_increment * i)),
                          1};
        vec3 scale = {circle_radius, circle_radius, 1}; 

        glm_translate(model, position);
        glm_scale(model, scale);

        glm_mat4_copy(model, models[i]);
        printf("#%2d location: (%3.0f, %3.0f)\n", i, position[0], position[1]);
    }

    char *uniform_names[] = {"cam_matrix", "models"};
    DrawDataType uniform_types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4};
    void *uniform_ptrs[] = {&cam_matrix[0][0], &models[0][0][0]};
    uint16_t lengths[] = {1, num_circles};

    wondererDrawerSetUniforms(circle, uniform_names, uniform_types, lengths,
                              uniform_ptrs, 2);    

}

int main(){
    if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE, \
                           WONDERER_WINDOW_FLOATING_FLAG))
        exit(-1);

    wondererEngineInit();

    // number of circles usecird
    const int num_circles = 3;

    Camera2D camera = {};
    wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);

    mat4 cam_matrix, models[num_circles];
    wondererCamera2DGetMatrix(&camera, cam_matrix);

    DrawData circles;
    initialize_circle(&circles, cam_matrix, models, num_circles);

    while(!wondererWindowShouldClose()){
        wondererWindowUpdate();
        wondererDrawerDrawByDataInstanced(&circles, num_circles);
    }

    wondererEngineDestroy();
    wondererDestroyWindow();
}