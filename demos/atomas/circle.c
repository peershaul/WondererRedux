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

// void update_circles(DrawData *circles_data, mat4 *models, 
//                     int *values, int circle_count){

//     // initializing values for the placement of the circles 
//     float angle_increment = 2 * PI / (circle_count - 1);

//     int half_width = INI_WIDTH / 2;
//     int half_height = INI_HEIGHT / 2;

//     // y axis => cos, x axis => sin
//     for(int i = 0; i < circle_count; i++){
//         mat4 model;
//         glm_mat4_identity(model);

//         vec3 position;
//         if(i > 0)
//             glm_vec3_copy((vec3){round(half_width  + CIRCLE_DISPLACEMENT_RADIUS * sin(angle_increment * (i - 1))),
//                                  round(half_height + CIRCLE_DISPLACEMENT_RADIUS * cos(angle_increment * (i - 1))),
//                                  1}, position);
//         else 
//             glm_vec3_copy((vec3){half_width, half_height, 1}, position);

//         vec3 scale = {CIRCLE_RADIUS, CIRCLE_RADIUS, 1}; 

//         glm_translate(model, position);
//         glm_scale(model, scale);

//         glm_mat4_copy(model, models[i]);
//     }

    
// }

// void update_circles_lengths(DrawData *data, int circle_count){
//     uint16_t *lengths = data->lengths.arr; 
//     lengths[1] = circle_count;
//     lengths[2] = circle_count;
// }

// void initialize_circles(DrawData *circles_data, mat4 cam_matrix, mat4 *models, 
//                         int *values, int circle_count){

//     // Getting the drawing information ready
//     *circles_data = (DrawData){
//         .shader = wondererShaderGet("demos/atomas/circle.vert",
//                                     "demos/atomas/circle.frag"),
//         .number_of_indices = sizeof(box_indices) / sizeof(uint8_t), 
//         .data_type_of_indices = GL_UNSIGNED_BYTE,
//         .texture_start_slot = 0
//     };

//     wondererDrawerGenerateLayout(circles_data, box_vertices, sizeof(box_vertices),
//                                  box_indices, sizeof(box_indices), 
//                                  box_layout_sections, sizeof(box_layout_sections));

//     // Generating start values for the circles 
//     for(int i = 0; i < circle_count; i++)
//         values[i] = rand() % 3 + 1;
//     for(int i = circle_count; i < CIRCLE_MAX_COUNT; i++)
//         values[i] = 0;

//     char *uniform_names[] = {"cam_matrix", "models", "values"};
//     DrawDataType uniform_types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4, 
//                                     WONDERER_DRAW_DATA_INT};
//     void *uniform_ptrs[] = {&cam_matrix[0][0], &models[0][0][0], values};
//     uint16_t lengths[] = {1, circle_count, circle_count};

//     update_circles(circles_data, models, values, circle_count);

//     wondererDrawerSetUniforms(circles_data, uniform_names, uniform_types, lengths,
//                               uniform_ptrs, 3);    

//     update_circles_lengths(circles_data, circle_count);
// }

static vec2 old_positions[CIRCLE_MAX_COUNT];
static vec2 new_positions[CIRCLE_MAX_COUNT];

static void calc_ring(vec2 positions[], int count){
    float angle_increment = 2 * PI / count;

    int half_height = INI_HEIGHT * 0.5;
    int half_width = INI_WIDTH * 0.5;

    for(int i = 0; i < count; i++)
        glm_vec2_copy((vec2){
            half_width  + CIRCLE_DISPLACEMENT_RADIUS * sin(angle_increment * i),
            half_height + CIRCLE_DISPLACEMENT_RADIUS * cos(angle_increment * i)
        }, positions[i]);
}

static float s_func(float x){
    float beta = 2;
    return 1 / (1 + powf(x / (1 - x), -beta));
}

static void lerp2(vec2 a, vec2 b, float amount, vec2 dest){
    float value = amount * amount * amount - 3 * amount * amount + 3 * amount;
    vec2 res = {
        a[0] + value * (b[0] - a[0]),
        a[1] + value * (b[1] - a[1])
    };

    glm_vec2_copy(res, dest);
}

void initialize_circles(CircleARR *circles, mat4 cam_matrix){

    // Initialize DrawData
    circles->data = (DrawData){
        .shader = wondererShaderGet("demos/atomas/circle.vert",
                                    "demos/atomas/circle.frag"),
        .number_of_indices = sizeof(box_indices) / sizeof(uint8_t),
        .data_type_of_indices = GL_UNSIGNED_BYTE,
        .texture_start_slot = 0
    };

    wondererDrawerGenerateLayout(&circles->data, box_vertices, sizeof(box_vertices),
                                 box_indices, sizeof(box_indices), 
                                 box_layout_sections, sizeof(box_layout_sections));

    // Initialize circle values  
    for(int i = 0; i < circles->count; i++)
        circles->values[i] = rand() % 3 + 1;
    for(int i = circles->count; i < CIRCLE_MAX_COUNT; i++)
        circles->values[i] = 0; 

    // setting up the uniforms
    char *uniform_names[] = {"cam_matrix", "models", "values"};
    DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4, 
                            WONDERER_DRAW_DATA_INT};
    uint16_t lengths[] = {1, circles->count, circles->count};
    void *ptrs[] = {&cam_matrix[0][0], &circles->models[0][0][0], circles->values};

    wondererDrawerSetUniforms(&circles->data, uniform_names, types, lengths, 
                              ptrs, 3);

    // Calculating the ring positions
    vec2 ring[circles->count - 1];
    calc_ring(ring, circles->count - 1);
    for(int i = 0; i < circles->count - 1; i++)
        glm_vec2_copy(ring[i], circles->positions[i]);

    // Positioning the center circle
    glm_vec2_copy((vec2){INI_WIDTH * 0.5, INI_HEIGHT * 0.5}, circles->positions[circles->count - 1]);
}

void update_circles_mats(CircleARR *circles){
    for(int i = 0; i < circles->count; i++){
        vec3 position = {circles->positions[i][0],
                         circles->positions[i][1],
                         1};
        
        vec3 scale = {CIRCLE_RADIUS, CIRCLE_RADIUS, 1};

        mat4 model;
        glm_mat4_identity(model);

        glm_translate(model, position);
        glm_scale(model, scale);

        glm_mat4_copy(model, circles->models[i]);
    }
}

void update_circle_count(CircleARR *circles, int new_count){
    circles->count = new_count;

    uint16_t *lengths = circles->data.lengths.arr;
    lengths[1] = new_count;
    lengths[2] = new_count;
}

bool add_circle(CircleARR *circles, int index){
    if(circles->count >= CIRCLE_MAX_COUNT - 1){
        printf("Max circle count reached!\n");
        return false;
    }

    int prev_center_value = circles->values[circles->count - 1];

    for(int i = circles->count - 1; i > index; i--){
        circles->values[i + 1] = circles->values[i];
        glm_vec2_copy(circles->positions[i], old_positions[i + 1]);
    }
    
    circles->values[index + 1] = prev_center_value;
    glm_vec2_copy((vec2){INI_WIDTH * 0.5, INI_HEIGHT * 0.5}, old_positions[index + 1]);

    circles->values[circles->count] = rand() % 6 + 1;

    for(int i = 0; i <= index; i++)
        glm_vec2_copy(circles->positions[i], old_positions[i]);

    // for(int i = 0; i < circles->count; i++)
    //     printf("(%3.0f, %3.0f)\t", circles->positions[i][0], circles->positions[i][1]);
    // printf("\n");

    // for(int i = 0; i < circles->count + 1; i++)
    //     printf("(%3.0f, %3.0f)\t", old_positions[i][0], old_positions[i][1]);
    // printf("\n");

    update_circle_count(circles, circles->count + 1);

    vec2 ring[circles->count - 1];
    calc_ring(ring, circles->count - 1);

    for(int i = 0; i < circles->count - 1; i++)
        glm_vec2_copy(ring[i], new_positions[i]);

    // glm_vec2_copy(ring[i], circles->positions[i]);
    
    glm_vec2_copy((vec2){INI_WIDTH * 0.5, INI_HEIGHT * 0.5}, 
                  circles->positions[circles->count - 1]);

    return true;
}

void animate_positions(CircleARR *circles, float elasped_time){
    float time = fmin(CIRCLE_ANIMATION_DURATION, elasped_time); 

    for(int i = 0; i < circles->count - 1; i++)
        lerp2(old_positions[i], new_positions[i], 
              time / CIRCLE_ANIMATION_DURATION, circles->positions[i]);
}