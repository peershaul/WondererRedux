#include "../../wonderer/include/engine.h"

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <cglm/cglm.h>

#include "main.h"

static int circles_count = 2;
static int circle_values[CIRCLE_MAX_COUNT] = {};
static DrawData circles;
static mat4 models[CIRCLE_MAX_COUNT];

static void push_center_circle(int index){
    if(circles_count >= CIRCLE_MAX_COUNT - 1){
        printf("Max circle count reached!\n");
        return;
    }

    for(int i = circles_count - 1; i > index; i--)
        circle_values[i + 1] = circle_values[i];        
    
    circle_values[index + 1] = circle_values[0]; 
    circle_values[0] = rand() % 3 + 1;

    circles_count++;

    update_circles(&circles, models, circle_values, circles_count);
    update_circles_lengths(&circles, circles_count);
} 

static void mouse_click_cb(int button, int action, int mods){
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        vec2 mouse_position;
        wondererWindowGetMousePos(mouse_position);

        vec2 screen_center = {INI_WIDTH / 2.0f, INI_HEIGHT / 2.0f};
        vec2 place_vector;

        glm_vec2_sub(mouse_position, screen_center, place_vector);

        float dist = sqrt(place_vector[0] * place_vector[0] + place_vector[1] * place_vector[1]); 
        float angle = asin(place_vector[0] / dist);
        if(place_vector[1] > 0)
            angle = PI - angle;
        else if(place_vector[1] < 0 && place_vector[0] < 0)
            angle = 2 * PI + angle;

        int index = (angle * (circles_count - 1) / (2 * PI)) + 1;

        if(dist >= CIRCLE_DISPLACEMENT_RADIUS / 2 && 
           dist <= CIRCLE_DISPLACEMENT_RADIUS * 2){

            push_center_circle(index);            
        }
    }
}

int main(){

    srand(time(NULL));

    if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE, \
                           WONDERER_WINDOW_FLOATING_FLAG))
        exit(-1);

    wondererEngineInit();

    wondererWindowAddCallback(WONDERER_WINDOW_MOUSE_PRESS_CALLBACK, mouse_click_cb);

    Camera2D camera = {};
    wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);

    mat4 cam_matrix; 
    wondererCamera2DGetMatrix(&camera, cam_matrix);

    initialize_circles(&circles, cam_matrix, models, circle_values,
                        circles_count);

    while(!wondererWindowShouldClose()){
        wondererWindowUpdate();
        wondererDrawerDrawByDataInstanced(&circles, circles_count);

        // ivec2 position = {};
        // wondererWindowGetMousePos(position);
        // printf("Mouse Position: (%4d, %4d)\n", position[0], position[1]);
    }

    wondererEngineDestroy();
    wondererDestroyWindow();
}