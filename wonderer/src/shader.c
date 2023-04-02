#include <GL/glew.h>

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>

#include "../include/mem_utils.h"
#include "../include/logger.h"

#include "internal/shader_funcs.c"

typedef struct {
   const char *vert_path, *frag_path;
   GLuint id;
} Shader;

#define WONDERER_MULTI_SHADER_SRC
#include "../include/shader.h"

static dynamic_arr shader_pool;
static Shader* bound = NULL;

void wondererShaderLoaderInit() {
   wondererDynamicArrayInitialize(4, sizeof(Shader), &shader_pool);
}

static Shader shader_create(const char *vert_path, const char *frag_path) {

   GLuint vert_shader = compile_shader(GL_VERTEX_SHADER, vert_path);
   GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER, frag_path);

   if(frag_shader == 0 || vert_shader == 0)
      return (Shader){};

   GLuint program = glCreateProgram();
   glAttachShader(program, vert_shader);
   glAttachShader(program, frag_shader);
   glLinkProgram(program);

   GLint linked;
   glGetProgramiv(program, GL_LINK_STATUS, &linked);
   if(linked != GL_TRUE){
      GLsizei log_length = 0;
      GLchar message[1024];
      glGetProgramInfoLog(program, 1024, &log_length, message);

      printf("SHADER LINK ERROR\nVertex Path: \"%s\" Fragment Path: \"%s\"\n%s\n",
             vert_path, frag_path, message);
      exit(0);
      return (Shader){};
   }

   glDeleteShader(vert_shader);
   glDeleteShader(frag_shader);
   return (Shader){.vert_path = vert_path, .frag_path = frag_path, .id = program};
}

Shader *wondererShaderCreate(const char *vert_path, const char *frag_path) {
   Shader* arr = shader_pool.arr;
   int32_t empty_space = -1;

   for(uint16_t i = 0; i < shader_pool.last; i++)
      if(arr[i].id == 0){
         empty_space = i;
         break;
      }

   if(empty_space != -1){
      arr[empty_space] = shader_create(vert_path, frag_path);
      Shader* ret = &arr[empty_space];

      return ret; 
   }

   if(shader_pool.last >= shader_pool.length){
      wondererDynamicArrayIncreaseSize(shader_pool.length * 2, &shader_pool);
      arr = shader_pool.arr;
   }

   arr[shader_pool.last] = shader_create(vert_path, frag_path);
   Shader* ret = &arr[shader_pool.last++];

   return ret; 
}

Shader *wondererShaderGet(const char *vert_path, const char *frag_path) {
   Shader* arr = shader_pool.arr;
   int32_t found = -1;

   for(uint16_t i = 0; i < shader_pool.last; i++)
      if(strcmp(vert_path, arr[i].vert_path) == 0 && 
         strcmp(frag_path, arr[i].frag_path) == 0){

         found = i;
         break;
      }

   if(found != -1){
      Shader* ret = &arr[found];
      WondererLog("Found Existing shader \"%s\" \"%s\"", vert_path, frag_path);
      return ret;
   }

   return wondererShaderCreate(vert_path, frag_path);
}

void wondererShaderDestroy(Shader *shader) {
   glDeleteProgram(shader->id);
   *shader = (Shader){};
}

void wondererShaderBind(Shader *shader) {
   bound = shader;
   if (shader != NULL) glUseProgram(shader->id);
   else glUseProgram(0);
}

void wondererShaderUnbind() {
   bound = NULL;
   glUseProgram(0);
}

GLuint wondererShaderGetID(Shader *shader) { return shader->id; }

Shader *wondererShaderGetBound(){ return bound; }

void wondererShaderLoaderTerminate() {
   wondererShaderUnbind();

   Shader* arr = shader_pool.arr;
   for(uint16_t i = 0; i < shader_pool.last; i++)
      glDeleteProgram(arr[i].id);

   free(shader_pool.arr);
   shader_pool = (dynamic_arr){};
}

void wondererShaderReload(Shader *shader) {
   const char* vert_path = shader->vert_path, *frag_path = shader->frag_path;
   bool is_bound = bound == shader;
   wondererShaderDestroy(shader);
   *shader = shader_create(vert_path, frag_path);
   if(bound)
      wondererShaderBind(shader);
}

void wondererShaderReloadAll() {
   Shader* arr = shader_pool.arr;
   for(uint16_t i = 0; i < shader_pool.last; i++){
      if(arr[i].id == 0) continue;
      wondererShaderReload(&arr[i]);
   }
}
