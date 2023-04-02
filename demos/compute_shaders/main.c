#include "../../wonderer/include/engine.h"

#include "../../wonderer/include/compute_shader.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#define WINDOW_TITLE "Wonderer | Compute shaders"

// TODO: overhaul all of the compute_shader.c mess, bc its not built right at all

void setup_window_quad(DrawData *quad, Texture *tex){

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


   *quad = (DrawData) {
      .shader = wondererShaderGet("demos/compute_shaders/post.vert",
                                  "demos/compute_shaders/post.frag"),
      .data_type_of_indices = GL_UNSIGNED_BYTE,
      .texture_start_slot = 0,
      .number_of_indices = sizeof(box_indices) / sizeof(uint8_t)
   };

   wondererDrawerGenerateLayout(quad, box_vertices, sizeof(box_vertices), 
                                box_indices, sizeof(box_indices), box_layout_sections, 
                                sizeof(box_layout_sections) / sizeof(uint8_t));

   char *uniform_names[] = {"tex"};
   DrawDataType types[] = {WONDERER_DRAW_DATA_TEXTURE};
   uint16_t lengths[] = {1};
   void *ptrs[] = {NULL};

   wondererDrawerSetUniforms(quad, uniform_names, types, lengths, ptrs, 1);
   wondererDrawerUploadTextures(quad, &tex, 1);
}

int main(){
   if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, WONDERER_WINDOW_FLOATING_FLAG))
      return -1;

   wondererEngineInit();
   wondererComputeShaderLoaderInit();

   vec3 color = {1, 0, 0};

   Texture *comp_tex = wondererTextureCreate();
   
   ComputeShader *comp_shader = 
      wondererComputeShaderCreate("demos/compute_shaders/compute.comp");
   
   DrawData window_quad;
   setup_window_quad(&window_quad, comp_tex);

   ComputeBindType bind_types[] = {WONDERER_COMPUTE_BIND_TEX2D};
   ivec3 bind_dimentions[] = {{WINDOW_WIDTH, WINDOW_HEIGHT}};
   void *bind_ptrs[] = {comp_tex};

   wondererComputeShaderSetBindings(comp_shader, bind_types, bind_dimentions,
                                    bind_ptrs, 1);

   DrawDataType types[] = {WONDERER_DRAW_DATA_VEC3};
   uint16_t lengths[] = {1};
   void *ptrs[] = {&color[0]};

   wondererComputeShaderSetUniforms(comp_shader, types, lengths, ptrs, 1);

   while(!wondererWindowShouldClose()){
      wondererWindowUpdate();

      wondererComputeShaderDispatch(comp_shader, (ivec3){
         ceil((float) WINDOW_WIDTH / 8),
         ceil((float) WINDOW_HEIGHT / 4),
         1
      });

      wondererDrawerDrawByData(&window_quad);      
   }


   wondererComputeShaderLoaderTerminate();
   wondererEngineDestroy();
}
