#include "imgui_basic.h"

#include "../../vendor/imgui/imgui.h"

extern "C"{ 
#include "../../wonderer/include/window.h"
#include "../../wonderer/include/camera2D.h"
}    

static bool show_demo = false;
static float f = 0.5;
static uint8_t counter = 0;

void wondererImguiSetupInspector(float dt, float *clear_color,
				 float *color_blend, Camera2D *cam,
				 struct ModelAttribs *attribs, bool *show_window) {
  // Creating a window
  if(*show_window){
    ImGui::Begin("Inspector", show_window);
    ImGui::Checkbox("Show Demo window", &show_demo);
    
    ImGui::ColorEdit3("clear color", clear_color);
    ImGui::DragFloat2("camera position", cam->position, 0.01f, -2000, 2000);
    ImGui::SliderFloat2("model position", attribs->position, -2000, 2000);
    ImGui::SliderFloat("model scale", &attribs->scale, 1, 1000);
    ImGui::SliderFloat("color blend", color_blend, 0, 1);
    
   
    ImGui::Text("FPS: %3.3f", 1 / dt);
    ImGui::End();
  }
  if(show_demo)
    ImGui::ShowDemoWindow(&show_demo);

}

