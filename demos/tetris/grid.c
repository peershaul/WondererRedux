#include "grid.h"

#include <cglm/affine.h>
#include <cglm/mat4.h>
#include <cglm/vec2.h>
#include <stdint.h>
#include <cglm/cglm.h>
#include <memory.h>

#include "../../wonderer/include/drawer.h"
#include "../../wonderer/include/layout.h"
#include "../../wonderer/include/shader.h"
#include "../../wonderer/include/camera2D.h"

#include "../../wonderer/include/logger.h"

#include "tetris.h"

#define SCREEN_PADDING 20
#define TILE_PADDING 2

void rotate_complex_int2(float *complex, uint8_t amount_of_rotations) {
  uint8_t amount = amount_of_rotations % 4;
  for(uint8_t i = 0; i < amount; i++){
    float copy[2] = {complex[0], complex[1]};
    complex[0] = -copy[1];
    complex[1] = copy[0];
  }
}

void calculate_matrices(struct Grid *grid, mat4 cam_matrix) {
  grid->scale = (INI_WIDTH - 2 * SCREEN_PADDING - 2 * GRID_COUNT_X * TILE_PADDING) /
    (2.0f * GRID_COUNT_X);
  WondererLog("Scale: %f", grid->scale);

  vec2 start_position, diff_position, curr_position;
  
  glm_vec2_copy((vec2){SCREEN_PADDING + TILE_PADDING + grid->scale,
		       SCREEN_PADDING + TILE_PADDING + grid->scale}, start_position);
  glm_vec2_copy((vec2){2 * (TILE_PADDING + grid->scale), 2 * (TILE_PADDING + grid->scale)},
		diff_position);

  glm_vec2_copy(start_position, curr_position);

  for(uint8_t y = 0; y < GRID_COUNT_Y; y++){
    for(uint8_t x = 0; x < GRID_COUNT_X; x++){
      mat4 *matrix = &grid->matrices[y * GRID_COUNT_X + x];

      glm_mat4_identity(*matrix);

      glm_translate(*matrix, (vec3){curr_position[0], curr_position[1], 0});
      glm_scale(*matrix, (vec3){grid->scale, grid->scale, 1});

      glm_mat4_mul(cam_matrix, *matrix, *matrix);

      curr_position[0] += diff_position[0];

    }
    curr_position[0] = start_position[0];
    curr_position[1] += diff_position[1];
  }
}

void tetris_grid_initialize(struct Grid *grid, Layout *layout, Camera2D *camera) {
  *grid = (struct Grid){
    .spots = {},
		.background = {0, 1, 0, 1, 1, 1, 0, 0, 2, 3, 2, 3, 4, 5, 2, 3, },
    .pieces = {
      (struct Piece){ // straight 
				.configuration = {{0, 1}, {0, 0}, {0, -1}, {0, -2}},
				.origin = {0.5, 0.5},
				.sticks = 0,
      },
      (struct Piece){ // square 
				.configuration = {{0, 0}, {-1, 0}, {0, -1}, {-1, -1}},
				.origin = {0.5, 0.5},
				.sticks = PIECE_HAS_LEFT_STICK
      },
      (struct Piece){ // L 
				.configuration = {{0, 0}, {0, 1}, {0, -1}, {1, -1}},
				.origin = {-1, 0},
				.sticks = PIECE_HAS_LEFT_STICK 
      },
      (struct Piece){ // Reverse L
				.configuration = {{0, 0}, {0, 1}, {0, -1}, {-1, -1}},
				.origin = {1, 0},
				.sticks = PIECE_HAS_RIGHT_STICK
      },
      (struct Piece){ // Z
				.configuration = {{0, 0}, {-1, 0}, {0, -1}, {1, -1}},
				.origin = {0, 1},
				.sticks = PIECE_HAS_LEFT_STICK | PIECE_HAS_RIGHT_STICK,
      },
      (struct Piece){ // Reversed Z
				.configuration = {{0, 0}, {1, 0}, {0, -1}, {-1, -1}},
				.origin = {0, 1},
				.sticks = PIECE_HAS_RIGHT_STICK | PIECE_HAS_LEFT_STICK,
      },
      (struct Piece){ // T
				.configuration = {{0, 0}, {-1, 0}, {1, 0}, {0, 1}},
				.origin = {0, 0},
				.sticks = PIECE_HAS_LEFT_STICK | PIECE_HAS_RIGHT_STICK,
      }
    },
    .colors = {
      {0, 0, 0},
      {0.647, 0.259, 0.259},
      {0.549, 0.580, 0.251},
      {0.870, 0.576, 0.372},
      {0.372, 0.506, 0.615},
      {0.521, 0.404, 0.561},
      {0.369, 0.553, 0.529},
      {0.812, 0.416, 0.298},
      {0.772, 0.784, 0.776},
    },
    .draw_data = {
      .shader = wondererShaderGet("demos/tetris/grid.vert", "demos/tetris/grid.frag"),
      .layout = layout,
      .number_of_indices = 6,
      .data_type_of_indices = GL_UNSIGNED_BYTE,
    }
  };

  wondererDrawerDrawDataInitializeLength(&grid->draw_data, 3);

  int* uniforms = grid->draw_data.uniforms.arr;
  DrawDataType *types = grid->draw_data.types.arr;
  uint16_t *lengths = grid->draw_data.lengths.arr;
  volatile void **data_ptrs = grid->draw_data.data_ptrs.arr;

  int shader_id = wondererShaderGetID(grid->draw_data.shader);
  
  uniforms[0] = glGetUniformLocation(shader_id, "trans_mats");
  uniforms[1] = glGetUniformLocation(shader_id, "spots");
  uniforms[2] = glGetUniformLocation(shader_id, "colors");

  types[0] = WONDERER_DRAW_DATA_MAT4_ARR;
  types[1] = WONDERER_DRAW_DATA_INT_ARR;
  types[2] = WONDERER_DRAW_DATA_VEC3_ARR;

  lengths[0] = GRID_COUNT_Y * GRID_COUNT_X;
  lengths[1] = GRID_COUNT_Y * GRID_COUNT_X;
  lengths[2] = 9;

  data_ptrs[0] = grid->matrices;
  data_ptrs[1] = grid->spots;
  data_ptrs[2] = grid->colors;

  wondererDrawerDrawDataCommitLength(&grid->draw_data, 3);

  mat4 cam_matrix;

  wondererCamera2DGetMatrix(camera, cam_matrix);

  calculate_matrices(grid, cam_matrix);
}

void tetris_grid_reset(struct Grid *grid) {
	for(uint16_t i = 0; i < GRID_COUNT_Y * GRID_COUNT_X; i++)
		grid->spots[i] = grid->background[i] = 0;
}

void tetris_grid_reset_spots(struct Grid *grid){
	for(uint16_t i = 0; i < GRID_COUNT_Y * GRID_COUNT_X; i++)
		grid->spots[i] = 0;
}

void tetris_grid_apply_background(struct Grid *grid){
	for(uint16_t i = 0; i < GRID_COUNT_X * GRID_COUNT_Y; i++)
		grid->spots[i] = grid->background[i];
}

void tetris_grid_draw(struct Grid *grid) {
  wondererDrawerDrawByDataInstanced(&grid->draw_data, GRID_COUNT_X * GRID_COUNT_Y);
}

uint8_t tetris_get_piece_coords(struct Piece *piece, uint8_t *origin_position, uint8_t rotation,
							uint8_t *coords, uint8_t *overflow_arr){
	uint8_t overflow_amount = 0;
	for(uint8_t i = 0; i < 4; i++){

		float rotation_coords[2] = {
			piece->configuration[i][0] + piece->origin[0],
			piece->configuration[i][1] + piece->origin[1]
		};

		rotate_complex_int2(rotation_coords, 3 - rotation);

		coords[2 * i + 0] = rotation_coords[0] + origin_position[0];
		coords[2 * i + 1] = rotation_coords[1] + origin_position[1];

		if(coords[2 * i + 0] < 0 || coords[2 * i + 0] > GRID_COUNT_X ||
		   coords[2 * i + 1] < 0 || coords[2 * i + 1] > GRID_COUNT_Y) 
			overflow_arr[overflow_amount++] = i;
	}

	return overflow_amount;
}

void tetris_grid_add_tetro(struct Grid *grid, uint8_t id, uint8_t *origin_position,
			   uint8_t rotation) {
  
  struct Piece* selected_piece = &grid->pieces[id];
  uint8_t coords[4 * 2], overflow[4], overflow_amount, ov_index = 0;
  overflow_amount = tetris_get_piece_coords(selected_piece, origin_position, 
			rotation, coords, overflow);

  for(uint8_t i = 0; i < 4; i++){
		if(overflow[ov_index] == i && overflow_amount > ov_index) {
			ov_index++;
			continue;
		}

    grid->spots[GRID_COUNT_X * coords[2 * i + 1] + coords[2 * i + 0]] = id + 1;
  }
}

void tetris_grid_remove_tetro(struct Grid *grid, uint8_t id,
                              uint8_t *origin_position, uint8_t rotation) {

	struct Piece *selected_piece = &grid->pieces[id];
	uint8_t coords[4 * 2], overflow[4], overflow_amount, ov_index = 0;
	overflow_amount = tetris_get_piece_coords(selected_piece, origin_position, 
			rotation, coords, overflow);

	for(uint8_t i = 0; i < 4; i++){
		if(overflow[ov_index] == i && overflow_amount > ov_index){
			ov_index++;
			continue;
		}

		grid->spots[GRID_COUNT_X * coords[2 * i + 1] + coords[2 * i + 0]] = 0;
	}
}


