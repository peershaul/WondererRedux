#ifndef WONDERER_MULTI_COMPUTE_SHADERS_H
#define WONDERER_MULTI_COMPUTE_SHADERS_H

#include <GL/glew.h>
#include <cglm/cglm.h>

#include "texture.h"
#include "drawer.h"

#ifndef WONDERER_MULTI_COMPUTE_SHADERS_SRC 
typedef void ComputeShader;
#endif

typedef enum {
   WONDERER_COMPUTE_BIND_TEX2D,
   WONDERER_COMPUTE_BIND_BUFFER
} ComputeBindType;

ComputeShader *wondererComputeShaderCreate(const char *path);
void wondererComputeShaderDestroy(ComputeShader* shader);

void wondererComputeShaderSetBindings(ComputeShader *shader, ComputeBindType *types, 
                                      ivec3 *dimentions, void **ptrs, int count);

void wondererComputeShaderSetUniforms(ComputeShader *shader, DrawDataType *types,
                                      uint16_t *lengths, void **data_ptrs, int count);

void wondererComputeShaderDispatch(ComputeShader *shader, ivec3 num_groups);

void wondererComputeShaderLoaderInit();
void wondererComputeShaderLoaderTerminate();
#endif
