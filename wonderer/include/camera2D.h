#ifndef WONDERER_MULTI_CAMERA_2D_H
#define WONDERER_MULTI_CAMERA_2D_H

#include <cglm/cglm.h>
#include <stdint.h>

typedef struct{
  vec2 position;
  mat4 projection;
} Camera2D;


void wondererCamera2DUpdateProjection(Camera2D *cam, uint16_t width, uint16_t height);
void wondererCamera2DUpdateView(Camera2D *cam, mat4 dest);

void wondererCamera2DGetMatrix(Camera2D *cam, mat4 cam_matrix);

#endif
