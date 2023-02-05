
#include "pong.h"

#include "../../vendor/imgui/imgui.h"
#include <cglm/vec3.h>

extern "C" {
  #include <cglm/cglm.h>
}

void ImguiInspector(float dt, float *clear_color, struct Ball *ball, struct Pong *pong,
                    bool *show_window, bool *stop_animation, bool *tie_color_to_ball) {
  
  if(*show_window){

    
    ImGui::Begin("Inspector", show_window);

    ImGui::Text("Ball");
    ImGui::ColorEdit3("Ball Color", ball->color);
    ImGui::DragFloat2("Ball Position", ball->position);
    ImGui::DragFloat("Ball Scale", &ball->scale);

    ImGui::Spacing();
    ImGui::Text("Pong");
    if(!*tie_color_to_ball) ImGui::ColorEdit3("Pong Color", pong->color);
    ImGui::DragFloat("Pong Horizotal Position", &pong->position[0]);
    ImGui::DragFloat2("Pong Scale", pong->scale);
    ImGui::Checkbox("Tie Color to ball", tie_color_to_ball); 

    ImGui::Spacing();
    ImGui::ColorEdit3("Clear color", clear_color);
    ImGui::Checkbox("Stop animation", stop_animation);
    
    ImGui::Text("FPS: %3.3f", 1 / dt);
    ImGui::End();
  }
}
