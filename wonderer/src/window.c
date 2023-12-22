#include "../include/window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "../include/mem_utils.h"
#include "../include/logger.h"

struct Window {
   uint16_t width, height;
   GLFWwindow* w;
   dynamic_arr resize_cb, key_cb, cursor_pos_cb, mouse_click_cb;
   const char* title;
};

static struct Window *window = NULL;

static void glfw_window_size_callback(GLFWwindow *w, int width, int height) {
   window->height = height;
   window->width = width;
   glViewport(0, 0, width, height);

   void (**arr)(uint16_t, uint16_t) = (void (**)(uint16_t, uint16_t)) window->resize_cb.arr;
   for(uint16_t i = 0; i < window->resize_cb.last; i++)
      (*arr[i])(width, height);
}

static void glfw_key_callback(GLFWwindow *w, int key, int scancode, int action,
                              int mods) {
   void(**arr)(int, int, int, int) = (void (**)(int, int, int, int)) window->key_cb.arr;

   for(uint16_t i = 0; i < window->key_cb.last; i++)
      (*arr[i])(key, scancode, action, mods);
}

static void glfw_cursor_pos_callback(GLFWwindow *w, double xpos, double ypos) {
   void(**arr)(double, double) = (void(**)(double, double)) window->cursor_pos_cb.arr;

   for(uint16_t i = 0; i < window->cursor_pos_cb.last; i++)
      (*arr[i])(xpos, ypos);
}

static void glfw_mouse_click_callback(GLFWwindow *w, int button, int action, int mods){
   void(**arr)(int, int, int) = window->mouse_click_cb.arr;

   for(int i = 0; i < window->mouse_click_cb.last; i++)
      (*arr[i])(button, action, mods);
}

bool wondererWindowInit(uint16_t width, uint16_t height, const char *title,
                        window_flags flags) {

   if(window != NULL){
      WondererError("Window already exists");
      return false;
   }

   if(!glfwInit()){
      WondererError("Cant Initialize GLFW");
      return false;
   }

   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
   glfwWindowHint(GLFW_SAMPLES, 16);

   if(flags & WONDERER_WINDOW_FLOATING_FLAG){
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
      glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
   }

   else if(flags & WONDERER_WINDOW_RESIZEABLE_FLAG)
      glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

   window = (struct Window*) malloc(sizeof(struct Window));
   window->width = width;
   window->height = height;
   window->title = title;

   window->w = glfwCreateWindow(width, height, title, NULL, NULL);
   if(!window->w){
      WondererError("Cant open GLFW window");
      glfwTerminate();
      free(window);
      window = NULL;
      return false;
   }

   if(flags & WONDERER_WINDOW_FLOATING_FLAG && flags & WONDERER_WINDOW_RESIZEABLE_FLAG)
      glfwSetWindowAttrib(window->w, GLFW_RESIZABLE, GLFW_TRUE);

   glfwMakeContextCurrent(window->w);

   if(glewInit() != GLEW_OK){
      WondererError("Cant Initialize GLEW");
      glfwDestroyWindow(window->w);
      glfwTerminate();
      free(window);
      window = NULL;
      return false;
   }

   wondererDynamicArrayInitialize(2, sizeof(void(*)(int, int, int, int)), &window->key_cb);
   wondererDynamicArrayInitialize(2, sizeof(void(*)(uint16_t, uint16_t)), &window->resize_cb);
   wondererDynamicArrayInitialize(2, sizeof(void(*)(double, double)), &window->cursor_pos_cb);
   wondererDynamicArrayInitialize(2, sizeof(void(*)(int, int, int)), & window->mouse_click_cb); 

   glfwSwapInterval(1);

   glfwSetWindowSizeCallback(window->w, glfw_window_size_callback);
   glfwSetKeyCallback(window->w, glfw_key_callback);
   glfwSetCursorPosCallback(window->w, glfw_cursor_pos_callback);
   glfwSetMouseButtonCallback(window->w, glfw_mouse_click_callback);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_MULTISAMPLE);

   glClearColor(0, 0, 0, 1);  

   return true;
}

void wondererDestroyWindow() {
   glfwDestroyWindow(window->w);
   glfwTerminate();
   free(window->key_cb.arr);
   free(window->resize_cb.arr);
   free(window);
   window = NULL;
}

GLFWwindow* wondererGetGLFWwindow(){ return window != NULL? window->w : NULL; }
bool wondererWindowShouldClose() {
   return window == NULL? true : glfwWindowShouldClose(window->w);
}

void wondererWindowGetDimentions(uint16_t *dimentions) {
   dimentions[0] = window->width;
   dimentions[1] = window->height;
}

void wondererWindowAddCallback(wonderer_window_cb_type type, void *cb) {
   switch(type){

      case WONDERER_WINDOW_KEY_CALLBACK: {
         if(window->key_cb.last == window->key_cb.length)
            wondererDynamicArrayIncreaseSize(window->key_cb.length * 2, &window->key_cb);

         void (**arr)(int, int, int, int) = (void (**)(int, int, int, int)) window->key_cb.arr;
         arr[window->key_cb.last++] = cb;
         return; }

      case WONDERER_WINDOW_RESIZE_CALLBACK:{
         if(window->resize_cb.last == window->resize_cb.length)
            wondererDynamicArrayIncreaseSize(window->resize_cb.length * 2, &window->resize_cb);

         void (**arr)(uint16_t, uint16_t) = (void (**)(uint16_t, uint16_t)) window->resize_cb.arr;
         arr[window->resize_cb.last++] = cb;
         return; } 

      case WONDERER_WINDOW_CURSOR_POS_CALLBACK:{
         if(window->cursor_pos_cb.last == window->cursor_pos_cb.length)
            wondererDynamicArrayIncreaseSize(window->cursor_pos_cb.length * 2, &window->cursor_pos_cb);

         void (**arr)(double, double) = (void (**)(double, double)) window->cursor_pos_cb.arr;
         arr[window->cursor_pos_cb.last++] = cb;
         return; }

      case WONDERER_WINDOW_MOUSE_PRESS_CALLBACK: {
         if(window->mouse_click_cb.last == window->mouse_click_cb.length)
            wondererDynamicArrayIncreaseSize(2 * window->mouse_click_cb.length, &window->cursor_pos_cb);

         void (**arr)(int, int, int) = window->mouse_click_cb.arr;
         arr[window->mouse_click_cb.last++] = cb;
         return;
      }

      default: WondererWarning("Callback type is not defined");
   }
}

void wondererWindowUpdate() {
   glfwPollEvents();
   glfwSwapBuffers(window->w);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void wondererWindowSetClearColor(vec3 color){
   glClearColor(color[0], color[1], color[2], 1);
}
