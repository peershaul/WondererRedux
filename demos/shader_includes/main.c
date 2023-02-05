#include "../../wonderer/include/engine.h"

#define HEIGHT 480
#define WIDTH 640
#define TITLE "Wonderer | shader includes"

static float box_vertices[] = {
	-1, -1, 
	-1,  1, 
	 1,  1, 
	 1, -1
};

static uint8_t box_indices[] = {
	1, 0, 2, 
	0, 3, 2
}; 

static uint8_t box_layout_sections[] = {2};


int main(){
	if(!wondererWindowInit(WIDTH, HEIGHT, TITLE, WONDERER_WINDOW_FLOATING_FLAG))
		return -1;

	wondererEngineInit();

	DrawData draw_data = {
		.number_of_indices = 6,
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.texture_start_slot = 0,
		.shader = wondererShaderGet("demos/shader_includes/res/default.vert",	                                   "demos/shader_includes/res/default.frag"),
	};

	wondererDrawerGenerateLayout(&draw_data, box_vertices, 
								sizeof(box_vertices), box_indices, 
								sizeof(box_indices), box_layout_sections,
								sizeof(box_layout_sections));

	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();
		wondererDrawerDrawByData(&draw_data);
	}

	wondererEngineDestroy();
}
