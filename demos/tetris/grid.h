#ifndef TETRIS_GRID_H
#define TETRIS_GRID_H

#include <stdint.h>
#include <cglm/cglm.h>
#include <stdbool.h>

#include "../../wonderer/include/drawer.h"
#include "../../wonderer/include/layout.h"
#include "../../wonderer/include/camera2D.h"

#include "tetris.h"

#define PIECE_HAS_LEFT_STICK  0x1 
#define PIECE_HAS_RIGHT_STICK 0x2

struct Piece {
  int8_t configuration[4][2];
  float origin[2];
	uint8_t sticks;
};

struct Grid {
  volatile int32_t spots[GRID_COUNT_X * GRID_COUNT_Y];
	volatile int32_t background[GRID_COUNT_X * GRID_COUNT_Y];
  mat4 matrices[GRID_COUNT_X * GRID_COUNT_Y];
  struct Piece pieces[7];
  vec3 colors[9];
  float scale;
  DrawData draw_data;
};

struct ControlledPiece {
	uint8_t position[2], rotation, id;
};

void tetris_grid_initialize(struct Grid* grid, Layout *layout, Camera2D *camera);
void tetris_grid_reset(struct Grid *grid);

void tetris_grid_reset_spots(struct Grid *grid);
void tetris_grid_apply_background(struct Grid *grid);

void tetris_grid_draw(struct Grid *grid);
void tetris_grid_add_tetro(struct Grid *grid, uint8_t id,
			   uint8_t *origin_position, uint8_t rotation);

void tetris_grid_remove_tetro(struct Grid *grid, uint8_t id,
			      uint8_t *origin_position, uint8_t rotation);

// if the coords are out of bounds then both axis are 0xffff for the specific out of bounds spot
uint8_t tetris_get_piece_coords(struct Piece *piece, uint8_t *origin_position,
			                    uint8_t rotation, uint8_t *coords, uint8_t *overflow_arr);

#endif
