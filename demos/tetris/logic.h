#ifndef TETRIS_LOGIC_H 
#define TETRIS_LOGIC_H

#include "grid.h" 
#include <stdint.h>
#include <stdbool.h>

void tetris_calculate_before_collision_point(struct ControlledPiece *c_piece, struct Grid *grid, uint8_t *collision_point);
bool tetris_if_piece_collided(struct ControlledPiece *c_piece, struct Grid *grid); 

#endif 
