#include "../../wonderer/include/logger.h"
#include "../../wonderer/include/window.h"

#include "../../wonderer/include/layout.h"
#include "../../wonderer/include/buffer.h"
#include "../../wonderer/include/shader.h"

#include "../../wonderer/include/imgui_cmpl.h"
#include "../../wonderer/include/mem_utils.h"

#include "../../wonderer/include/camera2D.h"

#include "pong.h"

#include <cglm/affine.h>
#include <cglm/cglm.h>
#include <cglm/mat4.h>

#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define INI_WIDTH 1280 
#define INI_HEIGHT 720 
#define INI_TITLE "Wonderer Multi | Pong"

#define PONG_MAX_SPEED 200

#define BALL_MAX_SPEED 500 
#define BALL_MIN_SPEED 200

static const float box_vertices[] = {
  -1, -1,
  -1,  1,
   1,  1,
   1, -1, 
};

static const uint8_t box_indices[] = {
  1, 0, 2,
  0, 3, 2
};


static const uint8_t box_layout_sections[] = {2};

static bool show_inspector = false;
static Camera2D camera = {};
static vec2 cursor_pos;

void key_callback(int key, int scancode, int action, int mods) {
  if(key == GLFW_KEY_Y && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL)
    show_inspector = !show_inspector;
}

void resize_callback(uint16_t width, uint16_t height) {
  wondererCamera2DUpdateProjection(&camera, width, height);
}

void mouse_move_callback(double posx, double posy) {
  glm_vec2_copy((vec2){posx, posy}, cursor_pos);
}

void engine_init() {
  wondererBufferLoaderInit();
  wondererShaderLoaderInit();
  wondererLayoutLoaderInit();
  wondererImguiInit();

  wondererWindowAddCallback(WONDERER_WINDOW_KEY_CALLBACK, key_callback);
  wondererWindowAddCallback(WONDERER_WINDOW_RESIZE_CALLBACK, resize_callback);
  wondererWindowAddCallback(WONDERER_WINDOW_CURSOR_POS_CALLBACK, mouse_move_callback);

  srand(time(0));
}

void engine_kill() {
  wondererImguiDestroy();
  wondererBufferLoaderTerminate();
  wondererShaderLoaderTerminate();
  wondererLayoutLoaderTerminate();
  wondererDestroyWindow();
}

void setup_box_layout(Layout **box_layout) {
  Buffer *ibo, *vbo;
  
  vbo = wondererBufferCreate(GL_ARRAY_BUFFER, sizeof(box_vertices), GL_STATIC_DRAW);
  wondererBufferFill(vbo, (void*) box_vertices, sizeof(box_vertices));

  ibo = wondererBufferCreate(GL_ELEMENT_ARRAY_BUFFER, sizeof(box_indices), GL_STATIC_DRAW);
  wondererBufferFill(ibo, (void*) box_indices, sizeof(box_indices));
  
  *box_layout = wondererLayoutCreate((uint8_t*) box_layout_sections,
				    sizeof(box_layout_sections) / sizeof(uint8_t), vbo, ibo);
}

void reset_ball(struct Ball *ball, bool initialized) {

  vec2 velocity = {100, 100};
  
  *ball = (struct Ball){
    .scale = 10,
    .color = {0.42, 0.365, 0.706},
    .position = {INI_WIDTH / 2.0f, INI_HEIGHT * 0.75},
  };

  ball->shader = wondererShaderGet("demos/pong/ball.vert", "demos/pong/ball.frag");

  glm_vec2_copy(velocity, ball->velocity);
  ball_randomize_velocity(ball);

  ball->uniforms[0] = glGetUniformLocation(wondererShaderGetID(ball->shader), "color");
  ball->uniforms[1] = glGetUniformLocation(wondererShaderGetID(ball->shader), "trans_matrix");
}

void reset_pong(struct Pong *pong) {
  float scale_y = 5;
  *pong = (struct Pong){
    .scale = {75, scale_y},
    .position = {INI_WIDTH / 2.0f, scale_y / 2},
    .color = {0.338, 0.520, 0.750},
  };

  pong->shader = wondererShaderGet("demos/pong/pong.vert", "demos/pong/pong.frag");

  pong->uniforms[0] = glGetUniformLocation(wondererShaderGetID(pong->shader), "trans_mat");
  pong->uniforms[1] = glGetUniformLocation(wondererShaderGetID(pong->shader), "color");
}

void update_pong(float dt, struct Pong *pong) {
  float pos_diff_x = cursor_pos[0] - pong->position[0];

  //float acceleration = 1.5 * pos_diff_x - pong->horizontal_velocity * 0.75;
  //pong->horizontal_velocity = pong->horizontal_velocity + acceleration * dt; 

  //float proposed_location = pong->horizontal_velocity * dt + pong->position[0];
  float proposed_location = cursor_pos[0]; 

  if(proposed_location > INI_WIDTH - pong->scale[0]){
    pong->position[0] = INI_WIDTH - pong->scale[0];
    pong->horizontal_velocity = 0;
  }
  else if(proposed_location < pong->scale[0]){
    pong->position[0] = pong->scale[0];
    pong->horizontal_velocity = 0;
  }
  else pong->position[0] = proposed_location;

}

bool update_ball(float dt, struct Ball *ball, struct Pong *pong) {
  vec2 dx;
  glm_vec2_scale(ball->velocity, dt, dx);

  vec2 result;
  glm_vec2_add(ball->position, dx, result);
  
  if(result[0] + ball->scale >= INI_WIDTH){
    result[0] = INI_WIDTH - ball->scale;
    ball->velocity[0] *= -1;
    ball_randomize_velocity(ball);
  }
  else if(result[0] - ball->scale <= 0){
    result[0] = ball->scale;
    ball->velocity[0] *= -1;
    ball_randomize_velocity(ball);
  }
  
  if(result[1] + ball->scale >= INI_HEIGHT){
    result[1] = INI_HEIGHT - ball->scale - pong->scale[1];
    ball->velocity[1] *= -1;
    ball_randomize_velocity(ball);
  }

  else if(result[1] - ball->scale <= 0){
    if(result[0] >= pong->position[0] - pong->scale[0] &&
       result[0] <= pong->position[0] + pong->scale[0]){
      WondererLog("Ball saved!");
      result[1] = ball->scale;
      ball->velocity[1] *= -1;
      ball_randomize_velocity(ball);
    }

    else return true; 
  }
  glm_vec2_copy(result, ball->position);
  return false;
}

void ball_randomize_velocity(struct Ball *ball) {
  vec2 direction = {
    ball->velocity[0] < 0? -1 : 1,
    ball->velocity[1] < 0? -1 : 1
  };

  
  int numbers[] = { rand(), rand() };

  double normalized_numbers[] = {
    numbers[0] / (double) INT_MAX,
    numbers[1] / (double) INT_MAX
  };

  float prev_speed = sqrt(ball->velocity[0] * ball->velocity[0] + ball->velocity[1] * ball->velocity[1]);
  const float speed_mul = 50;

  float speed = (normalized_numbers[0] * 2 - 1) * speed_mul + prev_speed;

  if(speed > BALL_MAX_SPEED) speed = BALL_MAX_SPEED;
  else if(speed < BALL_MIN_SPEED) speed = BALL_MIN_SPEED;

  normalized_numbers[1] = normalized_numbers[1] * 0.60 + 0.15;

  vec2 velocity;
  velocity[0] = speed * normalized_numbers[1];
  velocity[1] = sqrt(speed * speed - velocity[0] * velocity[0]);

  glm_vec2_mul(velocity, direction, velocity);
  glm_vec2_copy(velocity, ball->velocity);

  WondererLog("Ball Velocity: (%f, %f)", ball->velocity[0], ball->velocity[1]);
  WondererLog("Ball Velocity Cosine: %lf", normalized_numbers[1]);
  WondererLog("Ball Speed: %f", speed);
}

void draw_ball(struct Ball *ball, Layout* layout, mat4 cam_mat) {

  mat4 model, trans_mat;
  
  glm_mat4_identity(model);
  glm_translate(model, (vec3){ball->position[0], ball->position[1], 0});
  glm_scale(model, (vec3){ball->scale, ball->scale, 1});
  
  glm_mat4_mul(cam_mat, model, trans_mat);

  wondererShaderBind(ball->shader);
  glUniformMatrix4fv(ball->uniforms[1], 1, GL_FALSE, &trans_mat[0][0]);
  glUniform3f(ball->uniforms[0], ball->color[0], ball->color[1], ball->color[2]);
  
  wondererLayoutBind(layout);
  glDrawElements(GL_TRIANGLES, sizeof(box_indices) / sizeof(uint8_t),
		 GL_UNSIGNED_BYTE, NULL);
}

void draw_pong(struct Pong *pong, Layout *layout, mat4 cam_mat) {
  mat4 model, trans_mat;

  glm_mat4_identity(model);

  glm_translate(model, (vec3){pong->position[0], pong->position[1], 0});
  glm_scale(model, (vec3){pong->scale[0], pong->scale[1], 1});

  glm_mat4_mul(cam_mat, model, trans_mat);

  wondererShaderBind(pong->shader);
  glUniformMatrix4fv(pong->uniforms[0], 1, GL_FALSE, &trans_mat[0][0]);
  glUniform3f(pong->uniforms[1], pong->color[0], pong->color[1], pong->color[2]);
  
  wondererLayoutBind(layout);
  glDrawElements(GL_TRIANGLES, sizeof(box_indices) / sizeof(uint8_t),
		 GL_UNSIGNED_BYTE, NULL);
}

int main() {
  WondererLog("Hello World from pong");

  // Initializing a Wonderer Multi Window 
  if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE,
			  WONDERER_WINDOW_FLOATING_FLAG))
    return -1;

  // Updating the projection matrix of the 2D camera
  // according to the width and height of the window
  wondererCamera2DUpdateProjection(&camera, INI_WIDTH, INI_HEIGHT);
  
  // Initializing different memory structures that wonderer multi has 
  engine_init();

  // Initializing the Ball  
  struct Ball ball;
  reset_ball(&ball, true);

  // Initializing the Pong
  struct Pong pong;
  reset_pong(&pong);

  // Initializing the Blocks
  uint8_t wall_template[40] = {1, 1, 2, 2, 5, 2, 3, 4, 2, 3,
                               3, 2, 0, 4, 2, 0, 1, 3, 1, 3,
                               0, 0, 3, 4, 5, 1, 2, 4, 3, 2,
                               4, 4, 4, 2, 4, 3, 2, 3, 4, 2};

  struct Wall wall;

  reset_wall(&wall, 10, 40, wall_template);

  // Building the vertex layout of the simple box shaped elements in the game,
  // such as the ball and the pong
  Layout *box_layout;
  setup_box_layout(&box_layout);
 
  vec3 clear_color = {0.204, 0.153, 0.467};
  float startTime = glfwGetTime(), dt = -1, lastTime;
  bool stop_animation = false, tie_color_to_ball = true;

  vec2 ball_last_position;
  glm_vec2_copy(ball.position, ball_last_position);
  
  while(!wondererWindowShouldClose()){

    // Setting up the window for a new frame 
    wondererWindowUpdate();
    wondererImguiNewFrame();

    // Updating the game objects location according to the new frame
    if(!stop_animation && dt > 0){
      update_pong(dt, &pong);
      if(update_ball(dt, &ball, &pong)){
	reset_pong(&pong);
	reset_ball(&ball, false);
      }
      collision_detect_wall(&wall, &ball, ball_last_position, dt);
    }

    // Getting the camera matrix from the 2D camera
    mat4 cam_matrix;
    wondererCamera2DGetMatrix(&camera, cam_matrix);

    // Setting up the Imgui Inspector
    ImguiInspector(dt, clear_color, &ball, &pong, &show_inspector, &stop_animation,
		   &tie_color_to_ball);

    // Drawing the ball
    draw_ball(&ball, box_layout, cam_matrix);

    // Drawing the pong 
    draw_pong(&pong, box_layout, cam_matrix);

    // Draw the wall
    draw_wall(&wall, box_layout, cam_matrix);
    
    // Drawing the Imgui elements
    wondererImguiDraw();

    // Setting the clear color 
    glClearColor(clear_color[0], clear_color[1], clear_color[2], 1);
    
    if(tie_color_to_ball) glm_vec3_copy(ball.color, pong.color);

    glm_vec2_copy(ball.position, ball_last_position);
 
    lastTime = glfwGetTime();
    dt = lastTime - startTime;
    startTime = lastTime;
  }

  destroy_wall(&wall);
  engine_kill();
}
