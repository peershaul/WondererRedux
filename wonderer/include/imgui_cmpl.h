#ifndef WONDERER_MULTI_IMGUI_CMPL_H
#define WONDERER_MULTI_IMGUI_CMPL_H

#ifdef __cplusplus
extern "C"{
#endif 

#include "camera2D.h"
#include <cglm/cglm.h>
#include <stdbool.h>


void wondererImguiInit();
void wondererImguiDestroy();

void wondererImguiDraw();
void wondererImguiNewFrame();

//void wondererImguiShowDemoWindow(bool show); 

#ifdef __cplusplus
}
#endif

#endif
