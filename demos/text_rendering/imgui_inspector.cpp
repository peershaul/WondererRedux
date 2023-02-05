#include "imgui_inspector.h"

#include "../../vendor/imgui/imgui.h"

void imgui_inspector(bool *show_window, float dt, float *font_thresh){
	if(*show_window){
		ImGui::Begin("Inspector", show_window);
		
		ImGui::SliderFloat("Font Threshold", font_thresh, 0, 1);
		ImGui::Text("FPS: %f", 1 / dt);
		ImGui::End();
	}
}

