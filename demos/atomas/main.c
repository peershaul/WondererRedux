#include "../../wonderer/include/engine.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <cglm/cglm.h>

#include "main.h"

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

int main(){

    srand(time(NULL));

    if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE, \
                           WONDERER_WINDOW_FLOATING_FLAG))
        exit(-1);

    wondererEngineInit();

    // number of circles usecird
    int circles_count = 20;
    int circle_values[25] = {};

    Camera2D camera = {};
    wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);

    mat4 cam_matrix, models[circles_count];
    wondererCamera2DGetMatrix(&camera, cam_matrix);

    DrawData circles;
    initialize_circles(&circles, cam_matrix, models, circle_values,
                        circles_count);

    while(!wondererWindowShouldClose()){
        wondererWindowUpdate();
        wondererDrawerDrawByDataInstanced(&circles, circles_count);
    }

    wondererEngineDestroy();
    wondererDestroyWindow();
}