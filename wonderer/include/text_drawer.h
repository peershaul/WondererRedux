#ifndef WONDERER_MULTI_TEXT_DRAWER_H
#define WONDERER_MULTI_TEXT_DRAWER_H

#include <stdint.h>
#include <cglm/cglm.h>

#include "texture.h"
#include "mem_utils.h"
#include "drawer.h"
#include "shader.h"

typedef struct{
	int32_t cols;
	Texture *texture;
} BitmapFont;

typedef struct{
	uint16_t length, line_height, width;
	float threshold;
	BitmapFont font;
	dynamic_arr models, letters;
	DrawData data;
	vec2 position;
} TextBox;

void wondererGenerateTextBox(TextBox *text_box, const char *text, mat4 cam_matrix,
							vec2 position, Shader *shader, uint16_t line_height, 
							uint16_t width);

#endif
