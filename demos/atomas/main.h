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

void initialize_circles(DrawData *circles_data, mat4 cam_matrix, mat4 *models, 
                        int *circle_values, int circle_count);

void update_circles(DrawData *circles_data, mat4 *models, int *circle_values, int circle_count);

void update_circles_lengths(DrawData *data, int circle_count);

#endif