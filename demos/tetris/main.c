#include "../../wonderer/include/logger.h"
#include "../../wonderer/include/window.h"

#include "../../wonderer/include/camera2D.h"
#include "../../wonderer/include/layout.h"
#include "../../wonderer/include/buffer.h"

#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "grid.h"
#include "logic.h"
#include "tetris.h"

static Camera2D camera = {};
static int8_t buttons[5] = {-1, -1, -1, -1, -1};

void key_callback(int key, int scancode, int action, int mods){

		if(action == GLFW_PRESS)
			switch(key){
			case GLFW_KEY_UP:
				if(buttons[0] == -1) buttons[0] = 1;
				return;
			case GLFW_KEY_DOWN:
				if(buttons[1] == -1) buttons[1] = 1;
				return;
			case GLFW_KEY_RIGHT:
				if(buttons[2] == -1) buttons[2] = 1;
				return;
			case GLFW_KEY_LEFT:
				if(buttons[3] == -1) buttons[3] = 1;
				return;
			case GLFW_KEY_SPACE:
				if(buttons[4] == -1) buttons[4] = 1;
				return;
			}	

		if(action == GLFW_RELEASE)
			switch(key){
			case GLFW_KEY_UP:
				if(buttons[0] == 0) buttons[0] = -1;
				return;
			case GLFW_KEY_DOWN:
				if(buttons[1] == 0) buttons[1] = -1;
				return;
			case GLFW_KEY_RIGHT:
				if(buttons[2] == 0) buttons[2] = -1;
				return;
			case GLFW_KEY_LEFT:
				if(buttons[3] == 0) buttons[3] = -1;
				return;
			case GLFW_KEY_SPACE:
				if(buttons[4] == 0) buttons[4] = -1;
				return;
			}
}

void setup_box_layout(Layout **layout) {
  float box_vertices[] = {
    -1, -1,
    -1,  1,
     1,  1,
     1, -1
  };

  uint8_t box_indices[] = {
    1, 0, 2,
    0, 3, 2
  };

  uint8_t box_layout_sections[] = {2};

  Buffer *vbo, *ibo;

  vbo = wondererBufferCreate(GL_ARRAY_BUFFER, sizeof(box_vertices), GL_STATIC_DRAW);
  ibo = wondererBufferCreate(GL_ELEMENT_ARRAY_BUFFER, sizeof(box_indices), GL_STATIC_DRAW);

  wondererBufferFill(vbo, box_vertices, sizeof(box_vertices));
  wondererBufferFill(ibo, box_indices, sizeof(box_indices));

  *layout = wondererLayoutCreate(box_layout_sections, 1, vbo, ibo);
}

void engine_init() {
  wondererBufferLoaderInit();
  wondererLayoutLoaderInit();
  wondererShaderLoaderInit();

  srand(time(0));

	wondererWindowAddCallback(WONDERER_WINDOW_KEY_CALLBACK, key_callback);
}

void engine_destroy() {
  wondererBufferLoaderTerminate();
  wondererLayoutLoaderTerminate();
  wondererShaderLoaderTerminate();
}

int main() {
  // Creating and initializing the window
  if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE, WONDERER_WINDOW_FLOATING_FLAG))
    exit(1);

  engine_init();

  // Setting a background color
  glClearColor(0.2, 0.2, 0.2, 1);

  WondererLog("the size of the grid is: %d bytes", sizeof(struct Grid));

  wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);

  Layout *layout;
  setup_box_layout(&layout);

  struct Grid grid;
  tetris_grid_initialize(&grid, layout, &camera);

  struct ControlledPiece c_piece = {.position = {2, GRID_COUNT_Y - 3}}; 

  float startTime = glfwGetTime(), dt = -1.0f, lastTime;
  float down_timer = 0;
	const float down_timer_thresh = 0.05;

  // The main game loop 
  while(!wondererWindowShouldClose()){
    wondererWindowUpdate();
	
		tetris_grid_reset_spots(&grid);
		tetris_grid_apply_background(&grid);

		down_timer += dt;
		
		// Calculate collision spot
		uint8_t collision_coords[2], proj_coords[2 * 4], overflow[4], ov_index = 0, overflow_amount;
		tetris_calculate_before_collision_point(&c_piece, &grid, collision_coords);

		overflow_amount = tetris_get_piece_coords(&grid.pieces[c_piece.id], collision_coords, 
																							c_piece.rotation, proj_coords, overflow);

		// Getting user input
		if(buttons[0] == 1){ // UP
			buttons[0] = 0;
			c_piece.rotation = c_piece.rotation == 3? 0 : c_piece.rotation + 1;
		}

		if(buttons[1] == 1 || buttons[1] == 0){ // DOWN
			buttons[1] = 0;
			if(down_timer > down_timer_thresh || buttons[1]) {
				c_piece.position[1]--;
				down_timer = 0;
			}
		}
		else down_timer = 0;

		if(buttons[2] == 1){ // RIGHT
			buttons[2] = 0;
			struct Piece *piece = &grid.pieces[c_piece.id];

			if(piece->sticks & PIECE_HAS_RIGHT_STICK){
				if(c_piece.position[0] < GRID_COUNT_X - 2) 
					c_piece.position[0]++;
			}
			else if(c_piece.position[0] < GRID_COUNT_X - 1)
				c_piece.position[0]++;
		}

		if(buttons[3] == 1){ // LEFT
			buttons[3] = 0;
			struct Piece *piece = &grid.pieces[c_piece.id];

			if(piece->sticks & PIECE_HAS_LEFT_STICK){
				if(c_piece.position[0] > 1)
					c_piece.position[0]--;
			}
			else if(c_piece.position[0] > 0)
				c_piece.position[0]--;
		}

		if(buttons[4] == 1){ // SPACE 
			buttons[4] = 0;

			for(uint8_t i = 0; i < 4; i++)
				grid.background[GRID_COUNT_X * proj_coords[2 * i + 1] + proj_coords[2 * i + 0]] = 
					c_piece.id + 1; 

			c_piece = (struct ControlledPiece){
				.position = {GRID_COUNT_X / 2, GRID_COUNT_Y - 3},
				.rotation = 0,
				.id = rand() % 7,
			};	

			overflow_amount = tetris_get_piece_coords(&grid.pieces[c_piece.id], collision_coords, 
																							 c_piece.rotation, proj_coords, overflow);
		}

		// Drawing the predicted collision overlay
		for(uint8_t i = 0; i < 4; i++){
			if(overflow_amount > ov_index && overflow[ov_index] == i){
				ov_index++;
				continue;
			}
			grid.spots[GRID_COUNT_X * proj_coords[2 * i + 1] + proj_coords[2 * i + 0]] = 8;
		}

		// Drawing the controlled piece	
	  tetris_grid_add_tetro(&grid, c_piece.id, c_piece.position, c_piece.rotation);
		tetris_grid_draw(&grid);

		tetris_grid_remove_tetro(&grid, c_piece.id, c_piece.position, c_piece.rotation);

    lastTime = glfwGetTime();
    dt = lastTime - startTime;
    startTime = lastTime;
  }

  wondererDrawerDestroyDrawData(&grid.draw_data);

  // destorying the window
  wondererDestroyWindow();
  engine_destroy();
}

