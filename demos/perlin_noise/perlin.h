#ifndef PERLIN_H
#define PERLIN_H

#ifdef __cplusplus 
extern "C" {
#endif 

float get_perlin_value(float x, float y);

void generate_perlin_noise(float xoffset, float yoffset, 
                           int width, int height, 
                           float inc, int seed, 
                           int octaves, float *buff);

void Inspector(float dt, bool *show_window, bool *paint, 
               float *threshes);

#ifdef __cplusplus 
} 
#endif 

#endif 
