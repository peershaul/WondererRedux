#define WONDERER_MULTI_COMPUTE_SHADERS_SRC

#include <GL/glew.h>
#include <cglm/cglm.h>
#include <memory.h>

#include "../include/texture.h"
#include "../include/mem_utils.h"
#include "../include/shader.h"

struct BindingsArray {
   dynamic_arr types, ptrs, dimentions;
   int count;
};

struct UniformsArray {
   dynamic_arr types, ptrs, lengths;
   int count;
};

typedef struct {
   GLuint id;
   struct BindingsArray bindings;
   struct UniformsArray uniforms;
} ComputeShader;

#include "../include/compute_shader.h"

#include "internal/shader_funcs.c"

static dynamic_arr shader_pool = {};

void wondererComputeShaderLoaderInit(){
   wondererDynamicArrayInitialize(2, sizeof(ComputeShader), 
                                  &shader_pool);   
}

void wondererComputeShaderLoaderTerminate(){
   ComputeShader *arr = shader_pool.arr;

   for(int i = 0; i < shader_pool.last; i++)
      if(arr[i].id != 0)
         wondererComputeShaderDestroy(&arr[i]);
}

static ComputeShader create_shader(const char *path){
   GLuint shader_id = compile_shader(GL_COMPUTE_SHADER, path);
   if(shader_id == 0) return (ComputeShader) { .id = 0 }; 
   
   GLuint program = glCreateProgram();
   glAttachShader(program, shader_id);
   glLinkProgram(program);

   GLint status_ok;
   glGetProgramiv(program, GL_LINK_STATUS, &status_ok);
   if(status_ok != GL_TRUE){
      GLsizei log_length = 0;
      GLchar message[1024];
      glGetProgramInfoLog(program, 1024, &log_length, message);

      printf("COMPUTE SHADER LINK ERROR\nPath: \"%s\"\n%s\n", path, message);
      glDeleteShader(shader_id);
      exit(0);
   }

   glDeleteShader(shader_id);
   return (ComputeShader) { .id = program };
}

static int check_for_free_space(){
   ComputeShader *arr = shader_pool.arr;
   for(int i = 0; i < shader_pool.last; i++)
      if(arr[i].id == 0)
         return i;
   return -1;
}

static void initialize_bounded_tex2D(Texture *tex, ivec3 dimentions){
   GLuint id = wondererTextureGetID(tex);
   wondererTextureSetDimentions(tex, (ivec2){dimentions[0], dimentions[1]});

   WondererGLWrap(glTextureStorage2D(id, 1, GL_RGBA32F, dimentions[0], dimentions[1]));
   WondererGLWrap(glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F));
}

static void bind_data_tex2D(Texture *tex, int binding){
   glBindTextureUnit(0, wondererTextureGetID(tex));
}

static void upload_to_shader_by_type(int uniform, DrawDataType type, uint16_t length,
                                     void *data) {
   switch(type){

      case WONDERER_DRAW_DATA_INT:
         glUniform1iv(uniform, length, data);
         return;

      case WONDERER_DRAW_DATA_MAT4:
         glUniformMatrix4fv(uniform, length, GL_FALSE, data);
         return;

      case WONDERER_DRAW_DATA_VEC2:{
         glUniform2fv(uniform, length, data);
         return;
      }
      case WONDERER_DRAW_DATA_VEC3:{
         glUniform3fv(uniform, length, data);
         return;
      }

      case WONDERER_DRAW_DATA_FLOAT:
         glUniform1fv(uniform, length, data);
         return;

      default: 
         WondererError("Uploading this uniform is not supported yet");
         return; 
   }
}

ComputeShader *wondererComputeShaderCreate(const char *path){
   ComputeShader shader = create_shader(path);
   if(shader.id == 0) return NULL;
   
   int found = check_for_free_space();
   if(found == -1){
      if(shader_pool.last >= shader_pool.length)
         wondererDynamicArrayIncreaseSize(2 * shader_pool.last, 
                                          &shader_pool);

      ComputeShader *arr = shader_pool.arr;
      arr[shader_pool.last] = shader;
      return &arr[shader_pool.last++];
   }
   else{
      ComputeShader *arr = shader_pool.arr;
      arr[found] = shader;
      return &arr[found];
   }
}

void wondererComputeShaderSetUniforms(ComputeShader *shader, DrawDataType *types, 
                                      uint16_t *lengths, void **data_ptrs, 
                                      int count){

   shader->uniforms = (struct UniformsArray){}; 
   wondererDynamicArrayInitialize(count, sizeof(DrawDataType), 
                                  &shader->uniforms.types);

   wondererDynamicArrayInitialize(count, sizeof(void*),
                                  &shader->uniforms.ptrs);
   
   wondererDynamicArrayInitialize(count, sizeof(uint16_t), 
                                  &shader->uniforms.lengths);

   memcpy(shader->uniforms.types.arr, types, count * sizeof(DrawDataType));
   memcpy(shader->uniforms.ptrs.arr, data_ptrs, count * sizeof(void*));
   memcpy(shader->uniforms.lengths.arr, lengths, count * sizeof(uint16_t));

   shader->uniforms.types.last = count;
   shader->uniforms.ptrs.last = count;
   shader->uniforms.lengths.last = count;
}

void wondererComputeShaderSetBindings(ComputeShader *shader, ComputeBindType *types, 
                                      ivec3 *dimentions, void **ptrs, int count){

   shader->bindings = (struct BindingsArray){};

   wondererDynamicArrayInitialize(count, sizeof(ComputeBindType), 
                                  &shader->bindings.types);

   wondererDynamicArrayInitialize(count, sizeof(ivec3), 
                                  &shader->bindings.dimentions);
   
   wondererDynamicArrayInitialize(count, sizeof(void*), 
                                  &shader->bindings.ptrs);

   memcpy(shader->bindings.types.arr, types, count * sizeof(ComputeBindType));
   memcpy(shader->bindings.dimentions.arr, dimentions, count * sizeof(ivec3));
   memcpy(shader->bindings.ptrs.arr, ptrs, count * sizeof(void*));

   shader->bindings.types.last = count;
   shader->bindings.dimentions.last = count;
   shader->bindings.ptrs.last = count;

   // Initialize the binded objects
   for(int i = 0; i < count; i++){
      switch(types[i]){
         case WONDERER_COMPUTE_BIND_TEX2D:{
            initialize_bounded_tex2D((Texture*) ptrs[i], dimentions[i]);
            break;
         } 
         default: 
            WondererError("Initialization of the object at the index %d is not implemented yet", i);
         break;
      }
   } 
}

void wondererComputeShaderDispatch(ComputeShader *shader, ivec3 num_groups){
   Shader *curr_bound = wondererShaderGetBound();

   WondererGLWrap(glUseProgram(shader->id));

   DrawDataType *uniform_types = shader->uniforms.types.arr;
   uint16_t *uniform_lengths = shader->uniforms.lengths.arr;
   void **uniform_ptrs = shader->uniforms.ptrs.arr;
   
   for(int i = 0; i < shader->uniforms.types.last; i++){
      WondererGLWrap(upload_to_shader_by_type(i, uniform_types[i], 
                                              uniform_lengths[i], uniform_ptrs[i]));
   }

   WondererGLWrap(glDispatchCompute(num_groups[0], num_groups[1], num_groups[2]));
   WondererGLWrap(glMemoryBarrier(GL_ALL_BARRIER_BITS));

   void **binding_data = shader->bindings.ptrs.arr;
   ComputeBindType *binding_types = shader->bindings.types.arr;
   
   for(int i = 0; i < shader->bindings.ptrs.last; i++){
      switch(binding_types[i]){
         case WONDERER_COMPUTE_BIND_TEX2D:
            bind_data_tex2D((Texture*) binding_data[i], i);    
         break;
         default: 
            WondererError("An option to bind the item in index %d is not implemented", i);
         break;
      }
   }

   wondererShaderBind(curr_bound);
}

void wondererComputeShaderDestroy(ComputeShader *shader){
   glDeleteProgram(shader->id);
   shader->id = 0;

   wondererDynamicArrayFree(&shader->bindings.ptrs);
   wondererDynamicArrayFree(&shader->bindings.types);
   wondererDynamicArrayFree(&shader->bindings.dimentions);

   wondererDynamicArrayFree(&shader->uniforms.ptrs);
   wondererDynamicArrayFree(&shader->uniforms.types);
   wondererDynamicArrayFree(&shader->uniforms.lengths);
}

