#include "pong.h"

#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <stdint.h>
#include <stdbool.h>
#include <GL/glew.h>

#include <cglm/cglm.h>

#include "../../wonderer/include/shader.h"
#include "../../wonderer/include/layout.h"
#include "../../wonderer/include/mem_utils.h"
#include "../../wonderer/include/window.h"
#include "../../wonderer/include/logger.h"

#define WALL_PADDING 5

void populate_blocks(vec2 start_location, vec2 block_difference,
                     struct Wall* wall, uint8_t rows, uint8_t columns,
		     uint16_t templ_length,
		     uint8_t *template) {

  uint16_t temp_index = 0;
  vec2 curr_location;
  glm_vec2_copy(start_location, curr_location);
  struct Block *blocks = wall->blocks.arr;
  
  for(uint8_t x = 0; x < columns; x++){
    for(uint8_t y = 0; y < rows; y++){

      if(template[temp_index] != 0){
	blocks[wall->blocks.last].hardness = template[temp_index];
	glm_vec2_copy(curr_location, blocks[wall->blocks.last].position);
	wall->blocks.last++;
      }

      temp_index++;
      curr_location[0] += block_difference[0];
    }

    curr_location[0] = start_location[0];
    curr_location[1] += block_difference[1];
  }
}

void update_matrices(struct Wall *wall) {

  wall->matrices.last = 0;
  
  mat4* matrices = wall->matrices.arr;
  struct Block *blocks = wall->blocks.arr;

  for(uint8_t i = 0; i < wall->blocks.last; i++){
    if(blocks[i].hardness == 0) continue;

    glm_mat4_identity(matrices[wall->matrices.last]);

    glm_translate(matrices[wall->matrices.last],
		  (vec3){blocks[i].position[0], blocks[i].position[1], 0});

    glm_scale(matrices[wall->matrices.last],
	      (vec3){wall->global_scale[0], wall->global_scale[1], 1});

    wall->matrices.last++;
  }
}

void update_hardness(struct Wall *wall) {
  wall->hardness.last = 0;

  int32_t *arr = wall->hardness.arr;
  struct Block *blocks = wall->blocks.arr;

  for(uint8_t i = 0; i < wall->blocks.last; i++){
    if(blocks[i].hardness == 0) continue;

    arr[wall->hardness.last++] = blocks[i].hardness;
  }
}

void reset_wall(struct Wall *wall, uint8_t row_length,
		uint16_t template_length, uint8_t *template) {

  Shader* shader = wondererShaderGet("demos/pong/wall.vert", "demos/pong/wall.frag");
  GLuint shader_id = wondererShaderGetID(shader);

  // Initializing the wall struct
  *wall = (struct Wall){
    .global_scale = {40, 10},
    .global_color = {1, 1, 1},
    .shader = shader,
    .uniforms = {
      glGetUniformLocation(shader_id, "cam_mat"),
      glGetUniformLocation(shader_id, "trans_mats"),
      glGetUniformLocation(shader_id, "color_states"),
      glGetUniformLocation(shader_id, "hardness_values")
    }
  };

  // Counting the amount of bricks needed
  uint16_t brick_amount = 0;

  for(uint16_t i = 0; i < template_length; i++)
    if(template[i] != 0) brick_amount++;

  // Initializing the memory for the matrices and the blocks
  wondererDynamicArrayInitialize(brick_amount, sizeof(struct Block), &wall->blocks);
  wondererDynamicArrayInitialize(brick_amount, sizeof(mat4), &wall->matrices);
  wondererDynamicArrayInitialize(brick_amount, sizeof(int32_t), &wall->hardness);

  // Populating the bricks
  uint8_t columns = template_length / row_length;
  uint16_t window_dimentions[2];

  wondererWindowGetDimentions(window_dimentions);
  
  vec2 start_position = {
    window_dimentions[0] / 2.0f - (row_length - 1)  * (wall->global_scale[0] + WALL_PADDING),
    window_dimentions[1] / 2.0f - (columns - 1) * (wall->global_scale[1] + WALL_PADDING)
  };

  vec2 block_difference = {2 * (wall->global_scale[0] + WALL_PADDING),
			   2 * (wall->global_scale[1] + WALL_PADDING)};

  populate_blocks(start_position, block_difference, wall,
		  row_length, columns, template_length, template);

  update_matrices(wall);
  update_hardness(wall);
}

void aabb_ball_block(float ball_top_border, float ball_bottom_border,
                     float ball_left_border, float ball_right_border,
                     vec2 block_position, vec2 block_scale, bool *dest) {
 
  // Calculating the block collider
  float block_top_border = block_position[1] + block_scale[1];
  float block_bottom_border = block_position[1] - block_scale[1];
  float block_left_border = block_position[0] - block_scale[0];
  float block_right_border = block_position[0] + block_scale[0];
  
  // Seeing if there is a collision on some specific axis
  bool maybe_collided_top =
    block_top_border <= ball_top_border && block_top_border >= ball_bottom_border;
  bool maybe_collided_bottom =
    block_bottom_border >= ball_bottom_border && block_bottom_border <= ball_top_border;
  bool maybe_collided_right =
    block_right_border >= ball_right_border && block_left_border <= ball_right_border;
  bool maybe_collided_left =
    block_right_border >= ball_left_border && block_left_border <= ball_left_border;
  
  // Transfering the results fowards, to be compared,
  // essentially a collision in 2 axis is counted for a real collision
  dest[0] = maybe_collided_left || maybe_collided_right;
  dest[1] = maybe_collided_bottom || maybe_collided_top;
 
}

void collision_detect_wall(struct Wall *wall, struct Ball *ball,
			   vec2 last_ball_position, float dt) {
  struct Block *blocks = wall->blocks.arr;

  // Constructing the bounding box around the ball,
  // Esentially the collider of the ball
  float ball_top_border = ball->position[1] + ball->scale;
  float ball_bottom_border = ball->position[1] - ball->scale;
  float ball_left_border = ball->position[0] - ball->scale;
  float ball_right_border = ball->position[0] + ball->scale;

  uint8_t collision_mode = 0;
  int16_t index = -1;

  // Iterating across all the blocks and checking for collision 
  for(uint8_t i = 0; i < wall->blocks.last; i++){

    if(blocks[i].hardness == 0) continue;

    bool collisions_current[2];
    aabb_ball_block(ball_top_border, ball_bottom_border, ball_left_border, ball_right_border,
		    blocks[i].position, wall->global_scale, collisions_current);

    if(collisions_current[0] && collisions_current[1]){

      blocks[i].hardness--;

      float last_ball_top_border = last_ball_position[1] + ball->scale;
      float last_ball_bottom_border = last_ball_position[1] - ball->scale;
      float last_ball_left_border = last_ball_position[0] - ball->scale;
      float last_ball_right_border = last_ball_position[0] + ball->scale;

      // Reviewing last position recorded for ferther information
      bool collisions_last[2];
      aabb_ball_block(last_ball_top_border, last_ball_bottom_border,
		      last_ball_left_border, last_ball_right_border,
		      blocks[i].position, wall->global_scale, collisions_last);

      if(collision_mode == 0){
	if((collisions_last[0] && collisions_last[1])){
	  collision_mode = 3; // Collided on previous frame too
	}
	else if(collisions_last[1]){
	  collision_mode = 1; // Horizontal collision
	  index = i;
	}
	
	else if(collisions_last[0]){
	  collision_mode = 2; // Vertical collision
	  index = i;
	}
      }
    }
  }

  if(collision_mode == 3) {
    WondererLog("Collided previously");
    glm_vec2_scale(ball->velocity, -1, ball->velocity);
  }
  if(collision_mode == 1){
    ball->velocity[0] *= -1;

    vec2 last_position;
    glm_vec2_copy(ball->position, last_position);
    
    if(ball->position[0] < blocks[index].position[0])
      ball->position[0] = blocks[index].position[0] - wall->global_scale[0] - ball->scale;
    
    else 
      ball->position[0] = blocks[index].position[0] + wall->global_scale[0] + ball->scale;

    ball->position[1] = (ball->velocity[1] / ball->velocity[0]) *
      (ball->position[0] - last_position[0]) + last_position[1];
  }
  else if(collision_mode == 2){
    ball->velocity[1] *= -1;

    vec2 last_position;
    glm_vec2_copy(ball->position, last_position);
    
    if(ball->position[1] < blocks[index].position[1])
      ball->position[1] = blocks[index].position[1] - wall->global_scale[1] - ball->scale;

    else 
      ball->position[1] = blocks[index].position[1] + wall->global_scale[1] + ball->scale;

    ball->position[0] = (ball->velocity[0] / ball->velocity[1]) *
	(ball->position[1] - last_position[1]) + last_position[0];
  }

  if(collision_mode != 0){
    WondererLog("Collision detected, mode %d", collision_mode);

    update_matrices(wall);
    update_hardness(wall);
    ball_randomize_velocity(ball);
  }

}

void draw_wall(struct Wall *wall, Layout *layout, mat4 cam_matrix) {

  #define COLOR_STATES_COUNT 5

  vec3 color_states[COLOR_STATES_COUNT] = {
    {0, 0.5333, 1},
    {0.6667, 1, 0.4},
    {1, 0.4667, 0.4667},
    {0.4, 0.2667, 0},
    {0.7333, 0.7333, 0.7333}
  };

  wondererLayoutBind(layout);
  wondererShaderBind(wall->shader);

  glUniformMatrix4fv(wall->uniforms[0], 1, GL_FALSE, &cam_matrix[0][0]);
  glUniformMatrix4fv(wall->uniforms[1], wall->matrices.last, GL_FALSE, wall->matrices.arr);
  glUniform3fv(wall->uniforms[2], COLOR_STATES_COUNT, &color_states[0][0]);
  glUniform1iv(wall->uniforms[3], wall->hardness.last, wall->hardness.arr);
  
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL, wall->matrices.last);
}

void destroy_wall(struct Wall *wall) {
  free(wall->matrices.arr);
  free(wall->blocks.arr);
}
