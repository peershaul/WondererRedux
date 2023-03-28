#include "../../wonderer/include/camera3D.h"
#include "../../wonderer/include/engine.h"
#include "../../wonderer/include/obj_loader.h"

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define WINDOW_TITLE "Wonderer Multi OBJ Loader"

int main() {
   if (!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE,
                           WONDERER_WINDOW_FLOATING_FLAG))
      return -1;

   wondererEngineInit();

   Camera3D camera = {
      .position = {0, 0, 0},
      .look_at = {0, 1, 0},
      .up = {0, 0, 1}
   };

   wondererCamera3DUpdateProjection(&camera, glm_rad(60),
                                    (float) WINDOW_WIDTH / WINDOW_HEIGHT, 
                                    0.1, 1000);

   mat4 model, cam_matrix;
   wondererCamera3DGetMatrix(&camera, cam_matrix);

   DrawData data = {
      .shader = wondererShaderGet("demos/obj_loading/basic.vert",
                                  "demos/obj_loading/basic.frag"),
      .data_type_of_indices = GL_UNSIGNED_SHORT,
      .texture_start_slot = 0
   };

   ObjFormat formats[] = {
      {WONDERER_OBJ_FORMAT_LOCATION, 3},
      {WONDERER_OBJ_FORMAT_UV, 2},
   };

   wondererObjLoad("demos/obj_loading/untitled.obj", &data, formats, 2);

   char *names[] = {"cam_matrix", "model"};
   uint16_t lengths[] = {1 , 1};
   DrawDataType types[] = {WONDERER_DRAW_DATA_MAT4, WONDERER_DRAW_DATA_MAT4};
   void *data_ptrs[] = {&cam_matrix[0][0], &model[0][0]};

   wondererDrawerSetUniforms(&data, names, types, lengths, data_ptrs, 2);

   float startTime = glfwGetTime();
   float dt = -1, endTime;
 
   float curr_angle = 0;
   const float rotation_speed = 10; 
  
   while (!wondererWindowShouldClose()) {
      wondererWindowUpdate();

      glm_mat4_identity(model);

      if(dt > 0){
         curr_angle += dt * rotation_speed;

         glm_translate(model, (vec3){0, 50, 0});
         glm_rotate(model, glm_rad(curr_angle + 180), (vec3){0, 0, 1});
         glm_rotate(model, glm_rad(curr_angle), (vec3){0, 1, 0});
         glm_scale(model, (vec3){0.2, 0.2, 0.2});

         wondererDrawerDrawByData(&data);
         
         WondererLog("FPS %.3f", 1 / dt);
      }

      endTime = glfwGetTime();
      dt = endTime - startTime;
      startTime = endTime;
   }

   wondererEngineDestroy();
}
