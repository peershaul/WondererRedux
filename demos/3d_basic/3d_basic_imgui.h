#ifndef BASIC3D_IMGUI_H
#define BASIC3D_IMGUI_H 

#ifdef __cplusplus 
extern "C"{
#endif

#include <stdbool.h>
#include <cglm/cglm.h>
#include "../../wonderer/include/camera3D.h"

void imgui_inspector_window(bool *show, Camera3D *camera, float *radial_speed, float dt);

#ifdef __cplusplus 
}
#endif

#endif
