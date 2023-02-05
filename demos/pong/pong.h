#ifndef WONDERER_DEMO_PONG_H
#define WONDERER_DEMO_PONG_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <cglm/cglm.h>
#include <stdbool.h>

#include "../../wonderer/include/layout.h"
#include "../../wonderer/include/shader.h"
#include "../../wonderer/include/mem_utils.h"

  struct Ball {
    vec2 position, velocity;
    vec3 color;
    float scale;
    Shader *shader;
    int uniforms[2];
  };

  struct Pong{
    vec2 position, scale;
    vec3 color;
    Shader *shader;
    int uniforms[2];
    float horizontal_velocity;
  };

  struct Block {
    vec2 position;
    uint8_t hardness;
  };

  struct Wall {
    vec2 global_scale;
    vec3 global_color;
    dynamic_arr blocks, matrices, hardness;
    Shader *shader;
    int uniforms[4];
  };

  void ImguiInspector(float dt, vec3 clear_color, struct Ball *ball, struct Pong *pong,
		      bool *show_window, bool *stop_animation, bool *tie_color_to_ball); 

  void reset_wall(struct Wall *wall, uint8_t row_length, uint16_t template_length,
		  uint8_t* wall_template);

  void draw_wall(struct Wall* wall, Layout *layout, mat4 cam_matrix);
  void destroy_wall(struct Wall *wall);

  void collision_detect_wall(struct Wall *wall, struct Ball *ball,
			     vec2 last_ball_position, float dt);

  void ball_randomize_velocity(struct Ball *ball);


#ifdef __cplusplus
}
#endif

#endif
