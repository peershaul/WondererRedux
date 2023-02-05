#include "../include/camera3D.h"

#include <stdint.h>
#include <cglm/cglm.h>
#include <cglm/cam.h>

void wondererCamera3DUpdateProjection(Camera3D *cam, float fov_deg, 
		float aspect_ratio, float near, float far){
		glm_perspective(glm_rad(fov_deg), aspect_ratio, near, far, cam->projection);
}

void wondererCamera3DUpdateView(Camera3D *cam, mat4 dest){
	glm_mat4_identity(dest);
	
	glm_lookat(cam->position, cam->look_at, cam->up, dest);
}

void wondererCamera3DGetMatrix(Camera3D *cam, mat4 dest){
	mat4 view;

	wondererCamera3DUpdateView(cam, view);

	glm_mat4_identity(dest);
	glm_mat4_mul(cam->projection, view, dest);
}
