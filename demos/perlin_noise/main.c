#include "../../wonderer/include/engine.h"

#include "perlin.h"

#include <cglm/cglm.h>
#include <time.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1370
#define WINDOW_HEIGHT 900
#define WINDOW_TITLE "Wonderer | Perlin Noise"

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

void initialize_frame(DrawData *frame, uint16_t *tex){
   *frame = (DrawData){
      .shader = wondererShaderGet("demos/perlin_noise/frame.vert",
                                  "demos/perlin_noise/frame.frag"),
      .number_of_indices = sizeof(box_indices) / sizeof(uint8_t),
      .data_type_of_indices = GL_UNSIGNED_BYTE,
      .texture_start_slot = 0,
   };

   wondererDrawerGenerateLayout(frame, box_vertices, sizeof(box_vertices), 
                                box_indices, sizeof(box_indices), box_layout_sections, 
                                sizeof(box_layout_sections) / sizeof(uint8_t));
   
   int texture_slot = 0;

   char *frame_uniform_names[] = {"frame"};
   DrawDataType frame_types[] = {WONDERER_DRAW_DATA_TEXTURE};
   uint16_t frame_lengths[] = {1};
   void *frame_data_ptrs[] = {NULL};

   Texture *frame_texture = wondererTextureCreate(); 

   Texture *bound = wondererTextureGetBound(0);
   wondererTextureBind(texture_slot, frame_texture);
   
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
                WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RED, GL_UNSIGNED_SHORT, tex);
   
   wondererTextureBind(texture_slot, bound);

   wondererDrawerSetUniforms(frame, frame_uniform_names, frame_types, 
                             frame_lengths, frame_data_ptrs, 1);
   wondererDrawerUploadTextures(frame, &frame_texture, 1);
}

int main(){
   srand(time(NULL));
   if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, 
                          WONDERER_WINDOW_FLOATING_FLAG))
      return -1;

   wondererEngineInit();


   float perlin_noise_values[WINDOW_WIDTH * WINDOW_HEIGHT];
  
   generate_perlin_noise(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                         0.005, rand(), 4, perlin_noise_values);

   uint16_t perlin_noise_tex[WINDOW_WIDTH * WINDOW_HEIGHT];

   for(int y = 0; y < WINDOW_HEIGHT; y++)
      for(int x = 0; x < WINDOW_WIDTH; x++)
         perlin_noise_tex[y * WINDOW_WIDTH + x] = perlin_noise_values[y * WINDOW_WIDTH + x] * 0xffff;

   DrawData frame_data;
   initialize_frame(&frame_data, perlin_noise_tex);

   while(!wondererWindowShouldClose()){
      wondererWindowUpdate();

      wondererDrawerDrawByData(&frame_data);
   }

   wondererEngineDestroy();
}
