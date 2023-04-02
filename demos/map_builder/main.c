#include "../../wonderer/include/engine.h"

#include <stdint.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Wonderer | Map Builder"

static void make_buffer_quad(DrawData *fb_quad){
   float box_vertices[] = {
      -1, -1, 
      -1,  1, 
      1,  1, 
      1, -1
   };

   uint8_t box_indices[] = {
      1, 0, 2, 
      0, 3, 2
   }; 

   uint8_t box_layout_sections[] = {2};

   (*fb_quad) = (DrawData){
      .number_of_indices = sizeof(box_indices) / sizeof(uint8_t),
      .texture_start_slot = 0,
      .data_type_of_indices = GL_UNSIGNED_BYTE,
   };

   wondererDrawerGenerateLayout(fb_quad, box_vertices, sizeof(box_vertices), 
                                box_indices, sizeof(box_indices), box_layout_sections,
                                sizeof(box_layout_sections) / sizeof(uint8_t));

}

int main() {
   if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, WONDERER_WINDOW_FLOATING_FLAG))
      return -1;

   wondererEngineInit();

   DrawData fb_quad;
   make_buffer_quad(&fb_quad);

   while(!wondererWindowShouldClose()){
      wondererWindowUpdate();
   }

   wondererEngineDestroy();
}
