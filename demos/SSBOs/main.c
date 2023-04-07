#include "../../wonderer/include/engine.h"

#include <cglm/cglm.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Wonderer | SSBOs"

int main(){
   if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, WONDERER_WINDOW_FLOATING_FLAG))
      return -1;

   WondererLog("Hello world SSBOS");
   wondererEngineInit();

   int nums[] = {1, 2, 3, 4};

   ComputeShader *shader = wondererComputeShaderCreate("demos/SSBOs/compute.comp"); 
   Buffer *buff = wondererBufferCreate(GL_SHADER_STORAGE_BUFFER, sizeof(nums), 
                                       GL_DYNAMIC_COPY);   

   wondererBufferFill(buff, nums, sizeof(nums));
   
   ComputeBindType binding_type = WONDERER_COMPUTE_BIND_BUFFER;
   ivec3 binding_dimentions = {};
   
   wondererComputeShaderSetBindings(shader, &binding_type, &binding_dimentions, 
                                    &buff, 1);

   wondererComputeShaderDispatch(shader, (ivec3){4, 1, 1});

   wondererBufferGetData(buff, nums, sizeof(nums));
   
   for(int i = 0; i < 4; i++)
      printf("%d, ", nums[i]);
   printf("\n");

   while(!wondererWindowShouldClose()){
      wondererWindowUpdate();
   }

   wondererEngineDestroy();
}
