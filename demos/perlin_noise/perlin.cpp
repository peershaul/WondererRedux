#include "perlin.h"

#include "../../vendor/PerlinNoise/PerlinNoise.hpp"
#include "../../vendor/imgui/imgui.h"

extern "C"{

float get_perlin_value(float x, float y) {
   const siv::PerlinNoise::seed_type s = 123456u;
   siv::PerlinNoise perlin{ s };
   return perlin.octave2D_01(x * 0.01, y * 0.01, 4);    
}

void generate_perlin_noise(float xoffset, float yoffset, 
                           int width, int height, 
                           float inc, int seed, 
                           int octaves, float *buff){

   const siv::PerlinNoise::seed_type s = seed; 
   siv::PerlinNoise perlin { s };
   
   for(int y = 0; y < height; y++){
      for(int x = 0; x < width; x++){
         buff[y * width + x] = perlin.octave2D_01(xoffset + x * inc, 
                                                  yoffset + y * inc, 
                                                  octaves); 
      }
   } 
}

void Inspector(float dt, bool *show_window, bool *paint, 
               float *threshes){
   if(*show_window){
      ImGui::Begin("Inspector", show_window);
      
      ImGui::Checkbox("Paint", paint);
      ImGui::DragFloat3("Thresholds", threshes, 0.001, 0, 1);

      ImGui::Text("FPS: %f", 1 / dt);
      ImGui::End();

   }
}

}
