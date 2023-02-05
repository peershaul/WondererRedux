#include "3d_basic_imgui.h"

#include "../../vendor/imgui/imgui.h"

extern "C"{
	#include <cglm/cglm.h>
	#include <stdint.h>
  #include "../../wonderer/include/camera3D.h"
}

void imgui_inspector_window(bool *show, Camera3D *camera, float *radial_speed, float dt){
	if(*show){

		ImGui::Begin("Inspector", show);

		ImGui::SliderFloat3("Camera Position", camera->position, -25, 25);
		ImGui::SliderFloat("Radial Speed", radial_speed, 0, 10);

		ImGui::Text("Hello, World");
		ImGui::Text("FPS: %f", 1 / dt);

		ImGui::End();
	}
}
