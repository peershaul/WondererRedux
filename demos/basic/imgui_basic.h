#ifndef DEMO_BASIC_IMGUI_H
#define DEMO_BASIC_IMGUI_H

#ifdef __cplusplus
extern "C" {
  #endif 

#include "../../wonderer/include/camera2D.h"
#include <cglm/cglm.h>

    struct ModelAttribs{
    vec2 position;
    float scale;
  }; 


  void wondererImguiSetupInspector(float dt, float *clear_color,
				   float *color_blend, Camera2D *cam,
				   struct ModelAttribs* attribs, bool *show_window);
#ifdef __cplusplus
}
#endif

#endif 
