#include "logic.h"
#include "grid.h" 

#include <stdint.h>
#include <stdbool.h>

bool tetris_if_piece_collided(struct ControlledPiece *c_piece, struct Grid *grid){
	struct Piece *selected_piece = &grid->pieces[c_piece->id];
	uint8_t coords[2 * 4], overflow[4], ov_index = 0, overflow_amount;
	overflow_amount = tetris_get_piece_coords(selected_piece, c_piece->position, c_piece->rotation, coords, overflow);

	for(uint8_t i = 0; i < 4; i++){
		if(overflow_amount > ov_index && overflow[ov_index] == i){
			ov_index++;
			continue;
		}

		if(coords[2 * i + 1] == 0) 
			return true;

		else if(grid->background[GRID_COUNT_X * coords[2 * i + 1] + coords[2 * i + 0]] != 0) 
			return true;
	}
	return false;
}

void tetris_calculate_before_collision_point(struct ControlledPiece *c_piece, struct Grid *grid, uint8_t *collision_point){
	struct Piece *selected_piece = &grid->pieces[c_piece->id];

	uint8_t coords[2 * 4], overflow[4], ov_index = 0, overflow_amount;

	overflow_amount = tetris_get_piece_coords(selected_piece, c_piece->position, c_piece->rotation, coords, overflow);

	bool collided = false;			
	uint8_t passes = 0;

	while(!collided){
		for(uint8_t i = 0; i < 4; i++){
			if(overflow_amount > ov_index && overflow[ov_index] == i){
				ov_index++;
				
			if(coords[2 * i + 0] > GRID_COUNT_X || coords[2 * i + 1] > GRID_COUNT_Y)
					continue;
			}

			if(grid->background[GRID_COUNT_X * coords[2 * i + 1] + coords[2 * i + 0]] != 0){
				collided = true;
				passes--;
				break;
			}

			if(coords[2 * i + 1] == 0) 
				collided = true;
		}
		if(!collided){
			for(uint8_t i = 0; i < 4; i++) 
				coords[2 * i + 1]--;

			if(ov_index != 0){
				uint8_t curr_position[2] = {c_piece->position[0], c_piece->position[1] - passes};
				overflow_amount = tetris_get_piece_coords(selected_piece, curr_position, 
						c_piece->rotation, coords, overflow);
			 }
			
			passes++;
			ov_index = 0;
		}
	}

	collision_point[0] = c_piece->position[0];
	collision_point[1] = c_piece->position[1] - passes;
}
