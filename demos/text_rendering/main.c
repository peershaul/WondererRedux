#include "../../wonderer/include/engine.h"

#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdbool.h>
#include <cglm/cglm.h>

#include "imgui_inspector.h"

#define INI_WIDTH 1280
#define INI_HEIGHT 720

static int letter = '*' - 32;

struct TextBox{
	int line_height, length;
	dynamic_arr letters; // int32_t []
	dynamic_arr models; // mat4 []
	float threshold;
	Texture *font;
	DrawData data;
};

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

void generate_textbox(struct TextBox *textbox, int line_height, const char* text,
											mat4 cam_matrix, vec2 position){

	int length = strlen(text);

	textbox->data = (DrawData){
		.shader = wondererShaderGet("demos/text_rendering/text.vert", 
									"demos/text_rendering/text.frag"),
		.texture_start_slot = 0,
		.data_type_of_indices = GL_UNSIGNED_BYTE,
		.number_of_indices = sizeof(box_indices) / sizeof(uint8_t),
	};	
	
	textbox->length = length;
	textbox->line_height = line_height;
	textbox->threshold = 0.5f;
	wondererDynamicArrayInitialize(length, sizeof(int32_t), &textbox->letters);
	wondererDynamicArrayInitialize(length, sizeof(mat4), &textbox->models);
	float xpos = position[0] - (length * line_height * 0.5);

	int32_t *letters = textbox->letters.arr;
	mat4 *models = textbox->models.arr;

	for(int i = 0; i < length; i++){
		letters[textbox->letters.last++] = text[i] - 32;	
		vec3 curr_position = {xpos, position[1], 0};

		mat4 model;
		glm_mat4_identity(model);
		glm_translate(model, curr_position);
		glm_scale(model, (vec3){line_height * 0.5, line_height * 0.5, 1});
		
		glm_mat4_copy(model, models[textbox->models.last++]);
		
		xpos += line_height;
	}
		
	wondererDrawerGenerateLayout(&textbox->data, box_vertices, sizeof(box_vertices), 
															 box_indices, sizeof(box_indices), box_layout_sections,
															 sizeof(box_layout_sections) / sizeof(uint8_t));

	char *uniform_names[] = {"cam_matrix", "models", "letters", 
							"font", "threshold"};
	DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4,
							WONDERER_DRAW_DATA_INT, WONDERER_DRAW_DATA_TEXTURE,
							WONDERER_DRAW_DATA_FLOAT};
	uint16_t lengths[] = {1, length, length, 1, 1};
	void *ptrs[] = {&cam_matrix[0][0], textbox->models.arr, textbox->letters.arr, NULL,
									&textbox->threshold};

	wondererDrawerSetUniforms(&textbox->data, uniform_names, types, 
							  lengths, ptrs, 4);

	textbox->font = wondererTextureCreate();
	wondererTextureUploadTexImage(textbox->font, "demos/text_rendering/bitmap_font.png");

	wondererDrawerUploadTextures(&textbox->data, &textbox->font, 1);
}

int main(){
	if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, "Wonderer | Text Rendering", WONDERER_WINDOW_FLOATING_FLAG))
		return -1;

	wondererEngineInit();

	Camera2D camera = {};
	wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);
	mat4 cam_matrix;
	wondererCamera2DGetMatrix(&camera, cam_matrix);

	printf("textbox size: %ld bytes\n", sizeof(struct TextBox));
	struct TextBox text_box = {};// = generate_textbox(8, "Hello, World!", cam_matrix);
	generate_textbox(&text_box, 32, "PRETTY COOL RIGHT?", cam_matrix, 
									(vec2){INI_WIDTH * 0.5, INI_HEIGHT * 0.5});

	bool show_inspector = true;

	float startTime = glfwGetTime();
	float dt = -1, lastTime;

	while(!wondererWindowShouldClose()){
		wondererWindowUpdate();
		wondererImguiNewFrame();

		wondererDrawerDrawByDataInstanced(&text_box.data, text_box.length);

		imgui_inspector(&show_inspector, dt, &text_box.threshold);
		wondererImguiDraw();

		//WondererLog("Threashold: %f", text_box.threshold);
		
		lastTime = glfwGetTime();
		dt = lastTime - startTime;
		startTime = lastTime;
	}

	wondererEngineDestroy();
	wondererDestroyWindow();
}

