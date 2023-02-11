#include "orbiter.h"

#include "../../vendor/imgui/imgui.h" 

void Inspector(float dt, bool *show_window, float *world_height, vec2 COM, 
			   vec2 COM_velocity, vec2 camera_position, 
			   vec2 *body_positions, uint8_t body_count){

	if(*show_window){
		ImGui::Begin("Inspector", show_window);

		ImGui::SliderFloat("World Height", world_height, 300, 4000);
		
		ImGui::Text("COM: (%f, %f)", COM[0], COM[1]);
		ImGui::Text("COM velocity: (%f, %f)", COM_velocity[0], COM_velocity[1]);
		ImGui::Text("Camera Position: (%f, %f)", 
					 camera_position[0], camera_position[1]);

		for(int i = 0; i < body_count; i++)
			ImGui::Text("Body %d: (%f, %f)", 
						i + 1, body_positions[i][0], body_positions[i][1]);

		ImGui::Text("FPS: %f", 1 / dt);
		ImGui::End();
	}
}
