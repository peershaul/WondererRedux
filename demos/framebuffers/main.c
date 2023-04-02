#include "../../wonderer/include/camera3D.h"
#include "../../wonderer/include/engine.h"
#include "../../wonderer/include/framebuffer.h"
#include "../../wonderer/include/obj_loader.h"

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define WINDOW_WIDTH 2560
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "Wonderer Multi OBJ Loader"

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

int main() {
   if (!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE,
                           WONDERER_WINDOW_FLOATING_FLAG))
      return -1;

   wondererEngineInit();
   wondererFrameBufferLoaderInit();

   Camera3D camera = {
      .position = {0, 0, 0},
      .look_at = {0, 0, 1},
      .up = {0, 1, 0}
   };

   wondererCamera3DUpdateProjection(&camera, glm_rad(60),
                                    (float) WINDOW_WIDTH / WINDOW_HEIGHT, 
                                    0.1, 1000);

   mat4 model, cam_matrix;
   wondererCamera3DGetMatrix(&camera, cam_matrix);

   DrawData data = {
      .shader = wondererShaderGet("demos/framebuffers/basic.vert",
                                  "demos/framebuffers/basic.frag"),
      .data_type_of_indices = GL_UNSIGNED_SHORT,
      .texture_start_slot = 0
   };

   ObjFormat formats[] = {
      {WONDERER_OBJ_FORMAT_LOCATION, 3},
      {WONDERER_OBJ_FORMAT_UV, 2},
   };

   wondererObjLoad("demos/framebuffers/untitled.obj", &data, formats, 2);

   char *names[] = {"cam_matrix", "model"};
   uint16_t lengths[] = {1 , 1};
   DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4};
   void *data_ptrs[] = {&cam_matrix[0][0], &model[0][0]};

   wondererDrawerSetUniforms(&data, names, types, lengths, data_ptrs, 2);

   float startTime = glfwGetTime();
   float dt = -1, endTime;
 
   float curr_angle = 0;
   const float rotation_speed = 20; 
   
   FrameBuffer *fb = wondererFrameBufferCreate(WINDOW_WIDTH, WINDOW_HEIGHT);
   wondererFrameBufferSetClearColor(fb, (vec3){1, 0, 0});
  
   // The object that the frame buffer is drawn to
   DrawData fb_data = {
      .shader = wondererShaderGet("demos/framebuffers/post_process.vert",
                                  "demos/framebuffers/post_process.frag"),
      .number_of_indices = sizeof(box_indices) / sizeof(uint8_t),
      .texture_start_slot = 0,
      .data_type_of_indices = GL_UNSIGNED_BYTE
   };

   wondererDrawerGenerateLayout(&fb_data, box_vertices, sizeof(box_vertices), 
                                box_indices, sizeof(box_indices), box_layout_sections, 
                                sizeof(box_layout_sections) / sizeof(uint8_t));

   int texture_slot = 0;

   char *fb_uniform_names[] = {"frame"};
   DrawDataType fb_types[] = {WONDERER_DRAW_DATA_TEXTURE};
   uint16_t fb_lengths[] = {1};
   void *fb_data_ptrs[] = {NULL};

   Texture *fb_textures = wondererFrameBufferGetTexture(fb);

   wondererDrawerSetUniforms(&fb_data, fb_uniform_names, fb_types, fb_lengths, fb_data_ptrs, 1);
   wondererDrawerUploadTextures(&fb_data, &fb_textures, 1);
  
   while (!wondererWindowShouldClose()) {
      wondererWindowUpdate();


      if(dt > 0){
         curr_angle += dt * rotation_speed;

         glm_mat4_identity(model);

         glm_translate(model, (vec3){0, 0, 50});
         glm_rotate(model, glm_rad(curr_angle + 180), (vec3){0, 1, 0});
         glm_scale(model, (vec3){0.2, 0.2, 0.2});

         wondererFrameBufferBind(fb);
         wondererDrawerDrawByData(&data);

         wondererFrameBufferBind(NULL);
         wondererDrawerDrawByData(&fb_data);
         
         WondererLog("FPS %.3f", 1 / dt);
      }

      endTime = glfwGetTime();
      dt = endTime - startTime;
      startTime = endTime;
   }
   
   wondererFrameBufferLoaderTerminate(); 
   wondererEngineDestroy();
}
