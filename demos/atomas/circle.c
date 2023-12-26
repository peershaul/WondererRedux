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

void update_circles(DrawData *circles_data, mat4 *models, 
                    int *values, int circle_count){

    // initializing values for the placement of the circles 
    float angle_increment = 2 * PI / (circle_count - 1);

    int half_width = INI_WIDTH / 2;
    int half_height = INI_HEIGHT / 2;

    // y axis => cos, x axis => sin
    for(int i = 0; i < circle_count; i++){
        mat4 model;
        glm_mat4_identity(model);

        vec3 position;
        if(i > 0)
            glm_vec3_copy((vec3){round(half_width  + CIRCLE_DISPLACEMENT_RADIUS * sin(angle_increment * (i - 1))),
                                 round(half_height + CIRCLE_DISPLACEMENT_RADIUS * cos(angle_increment * (i - 1))),
                                 1}, position);
        else 
            glm_vec3_copy((vec3){half_width, half_height, 1}, position);

        vec3 scale = {CIRCLE_RADIUS, CIRCLE_RADIUS, 1}; 

        glm_translate(model, position);
        glm_scale(model, scale);

        glm_mat4_copy(model, models[i]);
    }

    
}

void update_circles_lengths(DrawData *data, int circle_count){
    uint16_t *lengths = data->lengths.arr; 
    lengths[1] = circle_count;
    lengths[2] = circle_count;
}

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

    char *uniform_names[] = {"cam_matrix", "models", "values"};
    DrawDataType uniform_types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4, 
                                    WONDERER_DRAW_DATA_INT};
    void *uniform_ptrs[] = {&cam_matrix[0][0], &models[0][0][0], values};
    uint16_t lengths[] = {1, circle_count, circle_count};

    update_circles(circles_data, models, values, circle_count);

    wondererDrawerSetUniforms(circles_data, uniform_names, uniform_types, lengths,
                              uniform_ptrs, 3);    

    update_circles_lengths(circles_data, circle_count);
}