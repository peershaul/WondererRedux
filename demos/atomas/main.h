#ifndef ATOMAS_MAIN_H
#define ATOMAS_MAIN_H

#include "../../wonderer/include/engine.h"

#define INI_WIDTH 1280
#define INI_HEIGHT 720
#define INI_TITLE "Wonderer | Atomas"

#define PI 3.14159f

#define CIRCLE_MAX_COUNT 25
#define CIRCLE_DISPLACEMENT_RADIUS 300
#define CIRCLE_RADIUS 50

#define CIRCLE_START_COUNT 3
#define CIRCLE_ANIMATION_DURATION 0.325f

typedef struct {
    DrawData data;

    int count;

    int values[CIRCLE_MAX_COUNT];
    vec2 positions[CIRCLE_MAX_COUNT];
    mat4 models[CIRCLE_MAX_COUNT];

} CircleARR;

// void initialize_circles(DrawData *circles_data, mat4 cam_matrix, mat4 *models, 
//                         int *circle_values, int circle_count);

// void update_circles(DrawData *circles_data, mat4 *models, int *circle_values, int circle_count);

// void update_circles_lengths(DrawData *data, int circle_count);


void initialize_circles(CircleARR *circles, mat4 cam_matrix);
void update_circles_mats(CircleARR *circles);
void update_circle_count(CircleARR *circles, int new_count);

bool add_circle(CircleARR *circles, int index);

void animate_positions(CircleARR *circles, float elasped_time);

#endif