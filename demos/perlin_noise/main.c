#include "../../wonderer/include/engine.h"

#include <cglm/cglm.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define WINDOW_TITLE "Wonderer | Perlin Noise"

#define NOISE_RESOLUTION 5

int main(){
   if(!wondererWindowInit(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, WONDERER_WINDOW_FLOATING_FLAG))
      return -1;

   wondererEngineInit();

   while(!wondererWindowShouldClose()){
      wondererWindowUpdate();
   }
   
   wondererEngineDestroy();
}
