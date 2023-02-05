#include "imgui.h"

#include "../../vendor/imgui/imgui.h"

void imguiInspector(float dt, bool *show_window, bool *refresh_platforms){
	if(*show_window){
		ImGui::Begin("Inspector", show_window);
		
		if(ImGui::Button("reload platforms"))
			*refresh_platforms = true;
		
		ImGui::Text("FPS: %f", 1 / dt);
		ImGui::End();
	}
}
