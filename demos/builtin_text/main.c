#include "../../wonderer/include/engine.h"
#include "../../wonderer/include/text_drawer.h"

#define INI_WIDTH 1280 
#define INI_HEIGHT 720

int main(){
	if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, "Engine Text Renderer Demo", 
							WONDERER_WINDOW_FLOATING_FLAG))
		return -1;

	wondererEngineInit();

	Camera2D camera = {};
	wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);
	mat4 cam_matrix;
	wondererCamera2DGetMatrix(&camera, cam_matrix);

	BitmapFont font = {
		16, wondererTextureGet("demos/builtin_text/bitmap_font.png")
	};
	TextBox textbox = {};
	
	Shader *shader = wondererShaderGet("demos/builtin_text/demo.vert", 
									   "demos/builtin_text/demo.frag");

	wondererGenerateTextBox(&textbox, "Hey this is test\nHello", cam_matrix, 
			(vec2){INI_WIDTH * 0.5, 10}, shader, 32, INI_WIDTH - 50);

	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();
		wondererDrawerDrawByDataInstanced(&textbox.data, textbox.length);
	}

	wondererEngineDestroy();
	wondererDestroyWindow();
}
