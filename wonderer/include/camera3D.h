#ifndef WONDERER_MULTI_CAMERA3D_H
#define WONDERER_MULTI_CAMERA3D_H

#include <stdint.h>
#include <cglm/cglm.h>

typedef struct {
	vec3 position, look_at, up;
	mat4 projection;
} Camera3D;

void wondererCamera3DUpdateProjection(Camera3D *cam, float fov_deg,
																			float aspect_ratio, float near, float far);
void wondererCamera3DUpdateView(Camera3D *cam, mat4 dest);

void wondererCamera3DGetMatrix(Camera3D *cam, mat4 cam_matrix);

#endif
