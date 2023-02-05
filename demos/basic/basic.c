#include <cglm/mat4.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../../wonderer/include/window.h"
#include "../../wonderer/include/logger.h"
#include "../../wonderer/include/shader.h"
#include "../../wonderer/include/buffer.h"
#include "../../wonderer/include/layout.h"
#include "../../wonderer/include/texture.h"

#include "../../wonderer/include/imgui_cmpl.h"
#include "../../wonderer/include/camera2D.h"

#include "imgui_basic.h"

#include <cglm/cglm.h>

#define INI_WIDTH  1280 
#define INI_HEIGHT 720 
#define INI_TITLE "Wonderer Multi | Basic"
#define CAMERA_SPEED 10

static bool show_imgui_window = false;
static mat4 projection_matrix;
static Camera2D cam;

static vec2 movement_direction = {};

void resize_callback(uint16_t width, uint16_t height) {
    WondererLog("Window resized (%dx%d)", width, height);
    wondererCamera2DUpdateProjection(&cam, width, height);
}

void key_callback(int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_R && action == GLFW_PRESS){
        WondererLog("Reloading all shaders");
        wondererShaderReloadAll();
    }
    
    if(key == GLFW_KEY_Y && action == GLFW_PRESS && mods & GLFW_MOD_CONTROL){
        WondererLog("Toggling Imgui Window");
        show_imgui_window = !show_imgui_window;
    }
    
    if(action == GLFW_PRESS){
        if(key == GLFW_KEY_W) movement_direction[1]--;
        else if(key == GLFW_KEY_S) movement_direction[1]++;
        else if(key == GLFW_KEY_A) movement_direction[0]++;
        else if(key == GLFW_KEY_D) movement_direction[0]--;
    }
    
    else if(action == GLFW_RELEASE){
        if(key == GLFW_KEY_W) movement_direction[1]++;
        else if(key == GLFW_KEY_S) movement_direction[1]--;
        else if(key == GLFW_KEY_A) movement_direction[0]--;
        else if(key == GLFW_KEY_D) movement_direction[0]++;
    }
}

int main() {
    WondererLog("Hello World From Wonderer Multi");
    
    wondererCamera2DUpdateProjection(&cam, INI_WIDTH, INI_HEIGHT);
    
    if(!wondererWindowInit(INI_WIDTH, INI_HEIGHT, INI_TITLE,
                           WONDERER_WINDOW_RESIZEABLE_FLAG | WONDERER_WINDOW_FLOATING_FLAG))
        return -1;
    
    wondererShaderLoaderInit();
    wondererBufferLoaderInit();
    wondererLayoutLoaderInit();
    wondererTextureLoaderInit();
    
    wondererWindowAddCallback(WONDERER_WINDOW_RESIZE_CALLBACK, resize_callback);
    wondererWindowAddCallback(WONDERER_WINDOW_KEY_CALLBACK, key_callback);
    
    Shader* shader = wondererShaderGet("demos/basic/basic.vert", "demos/basic/basic.frag");
    
    wondererShaderBind(shader);
    
    float square_vertices[] = {
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,  0, 0, // Bottom Left 
        -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  0, 1, // Top Left 
        0.5f,  0.5f, 0.0f, 0.0f, 1.0f,  1, 1, // Top Right 
        0.5f, -0.5f, 1.0f, 0.0f, 1.0f,  1, 0  // Bottom Right
    };
    
    uint8_t square_indices[] = {
        1, 0, 2,
        0, 3, 2
    }; 
    
    uint8_t layout_sections[] = {2, 3, 2};
    
    Buffer *vbo, *ibo;
    Layout *square_layout;
    
    vbo = wondererBufferCreate(GL_ARRAY_BUFFER, sizeof(square_vertices), GL_STATIC_DRAW);
    wondererBufferFill(vbo, square_vertices, sizeof(square_vertices));
    
    ibo = wondererBufferCreate(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_indices), GL_STATIC_DRAW);
    wondererBufferFill(ibo, square_indices, sizeof(square_indices));
    
    square_layout = wondererLayoutCreate(layout_sections,
                                         sizeof(layout_sections) / sizeof(uint8_t),
                                         vbo, ibo);
    wondererImguiInit();
    
    float startTime = glfwGetTime(), dt = -1, lastTime;
    float clear_color[3] = {0, 0, 0};
    float color_blend = 0;
    
    Texture *tex = wondererTextureCreate();
    wondererTextureBind(0, tex);
    
    wondererTextureUploadTexImage(tex, "demos/basic/texture.jpeg");
    
    int texture_uniform_location = glGetUniformLocation(wondererShaderGetID(shader), "tex");
    int cam_mat_uniform_location = glGetUniformLocation(wondererShaderGetID(shader), "cam_mat");
    int color_blend_uniform_location = glGetUniformLocation(wondererShaderGetID(shader), "color_blend");
    
    glUniform1i(texture_uniform_location, 0);
    
    mat4 cam_matrix;
    mat4 model_matrix;
    
    struct ModelAttribs model_attribs = {{100, 100}, 200};
    
    
    while(!wondererWindowShouldClose()){
        wondererWindowUpdate();
        
        float movement_vec_length = sqrt(movement_direction[0] * movement_direction[0] +
                                         movement_direction[1] * movement_direction[1]);
        
        if(movement_vec_length != 0){
            vec2 dx = {
                movement_direction[0] * CAMERA_SPEED / movement_vec_length,
                movement_direction[1] * CAMERA_SPEED / movement_vec_length
            };
            glm_vec2_add(dx, cam.position, cam.position);
        }
        
        glm_mat4_identity(model_matrix);
        glm_translate(model_matrix, (vec3){model_attribs.position[0], model_attribs.position[1], 0});
        glm_scale(model_matrix, (vec3){model_attribs.scale, model_attribs.scale, 1});
        
        
        wondererCamera2DGetMatrix(&cam, cam_matrix);
        glm_mat4_mul(cam_matrix, model_matrix, cam_matrix);
        
        wondererLayoutBind(square_layout);
        glUniformMatrix4fv(cam_mat_uniform_location, 1, GL_FALSE, &cam_matrix[0][0]);
        glUniform1f(color_blend_uniform_location, color_blend);
        glDrawElements(GL_TRIANGLES, sizeof(square_indices) / sizeof(uint8_t),
                       GL_UNSIGNED_BYTE, NULL);
        
        wondererImguiNewFrame();
        
        if(dt > 0){
            wondererImguiSetupInspector(dt, clear_color, &color_blend, &cam, &model_attribs, &show_imgui_window);
            glClearColor(clear_color[0], clear_color[1], clear_color[2], 1);
        }
        
        wondererImguiDraw();
        
        lastTime = glfwGetTime();
        dt = lastTime - startTime;
        startTime = lastTime;
    }
    
    wondererImguiDestroy();
    
    wondererBufferLoaderTerminate();
    wondererLayoutLoaderTerminate();
    wondererShaderLoaderTerminate();
    wondererTextureLoaderTerminate();
    wondererDestroyWindow();
}

