#ifndef ORBITER_H
#define ORBITER_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdbool.h>
#include <stdint.h>
#include <cglm/cglm.h>

void Inspector(float dt, bool *show_window, float *world_height, vec2 COM, 
			   vec2 COM_velocity, vec2 camera_position, vec2 *body_positions, 
			   uint8_t body_count);

#ifdef __cplusplus
}
#endif 

#endif 
