#ifndef WONDERER_MULTI_WINDOW_H
#define WONDERER_MULTI_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t window_flags;

#define WONDERER_WINDOW_RESIZEABLE_FLAG 0x1
#define WONDERER_WINDOW_FLOATING_FLAG   0x2

typedef enum {
  WONDERER_WINDOW_RESIZE_CALLBACK,
  WONDERER_WINDOW_KEY_CALLBACK,
  WONDERER_WINDOW_CURSOR_POS_CALLBACK,
  WONDERER_WINDOW_MOUSE_PRESS_CALLBACK,
} wonderer_window_cb_type;

bool wondererWindowInit(uint16_t width, uint16_t height, const char* title, window_flags flags);
void wondererDestroyWindow();

GLFWwindow *wondererGetGLFWwindow();
bool wondererWindowShouldClose();
void wondererWindowGetDimentions(uint16_t* dimentions);

void wondererWindowAddCallback(wonderer_window_cb_type type, void* cb);

void wondererWindowSetClearColor(vec3 color);

void wondererWindowUpdate();

#endif
