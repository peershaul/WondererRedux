#include "main.h"

#include <stdlib.h>
#include <stdio.h>

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

void initialize_circles(DrawData *circles_data, mat4 cam_matrix, mat4 *models, 
                        int *values, int circle_count){

    // Getting the drawing information ready
    *circles_data = (DrawData){
        .shader = wondererShaderGet("demos/atomas/circle.vert",
                                    "demos/atomas/circle.frag"),
        .number_of_indices = sizeof(box_indices) / sizeof(uint8_t), 
        .data_type_of_indices = GL_UNSIGNED_BYTE,
        .texture_start_slot = 0
    };

    wondererDrawerGenerateLayout(circles_data, box_vertices, sizeof(box_vertices),
                                 box_indices, sizeof(box_indices), 
                                 box_layout_sections, sizeof(box_layout_sections));

    // Generating start values for the circles 
    for(int i = 0; i < circle_count; i++)
        values[i] = rand() % 3 + 1;
    for(int i = circle_count; i < CIRCLE_MAX_COUNT; i++)
        values[i] = 0;

    for(int i = 0; i < CIRCLE_MAX_COUNT; i++)
        printf("%d, ", values[i]);
    printf("\n");

    // initializing values for the placement of the circles 
    float angle_increment = 2 * PI / circle_count;

    int half_width = INI_WIDTH / 2;
    int half_height = INI_HEIGHT / 2;

    int displacement_radius = 300;
    int circle_radius = 50;

    // y axis => cos, x axis => sin
    for(int i = 0; i < circle_count; i++){
        mat4 model;
        glm_mat4_identity(model);

        vec3 position =  {round(half_width  + displacement_radius * sin(angle_increment * i)),
                          round(half_height + displacement_radius * cos(angle_increment * i)),
                          1};
        vec3 scale = {circle_radius, circle_radius, 1}; 

        glm_translate(model, position);
        glm_scale(model, scale);

        glm_mat4_copy(model, models[i]);
    }

    char *uniform_names[] = {"cam_matrix", "models", "values"};
    DrawDataType uniform_types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4, 
                                    WONDERER_DRAW_DATA_INT};
    void *uniform_ptrs[] = {&cam_matrix[0][0], &models[0][0][0], values};
    uint16_t lengths[] = {1, circle_count, circle_count};

    wondererDrawerSetUniforms(circles_data, uniform_names, uniform_types, lengths,
                              uniform_ptrs, 3);    

}