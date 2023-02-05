#include "../include/camera2D.h"

#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <stdint.h>

void wondererCamera2DUpdateProjection(Camera2D *cam, uint16_t width, uint16_t height){
  glm_mat4_identity(cam->projection);
  glm_ortho(0, width, 0, height, 0, 1000, cam->projection);
}

void wondererCamera2DUpdateView(Camera2D *cam, mat4 dest) {
  glm_mat4_identity(dest);

  vec3 cameraFront = {0, 0, -1};
  vec3 cameraUp = {0, 1, 0};
  vec3 cameraPosition = {cam->position[0], cam->position[1], 20};

  vec3 eye;
  glm_vec3_add(cameraFront, (vec3){cameraPosition[0], cameraPosition[1], 0}, eye);
  glm_lookat(cameraPosition, eye, cameraUp, dest);
}

void wondererCamera2DGetMatrix(Camera2D* cam, vec4 *cam_matrix) {
  mat4 view;
  wondererCamera2DUpdateView(cam, view);

  glm_mat4_mul(cam->projection, view, cam_matrix);
}
