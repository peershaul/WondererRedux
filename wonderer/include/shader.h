#ifndef WONDERER_MULTI_SHADER_H
#define WONDERER_MULTI_SHADER_H

#ifndef WONDERER_MULTI_SHADER_SRC
typedef void Shader;
#endif

#include <GL/glew.h>

void wondererShaderLoaderInit();
void wondererShaderLoaderTerminate();

void wondererShaderBind(Shader *shader);
Shader *wondererShaderGetBound();
void wondererShaderUnbind();

GLuint wondererShaderGetID(Shader* shader);

void wondererShaderReload(Shader *shader);
void wondererShaderReloadAll();

// Always creates a new shader without to check if it already exists
Shader *wondererShaderCreate(const char *vert_path, const char *frag_path);

// Checks if a shader exists then returns it if it is or creates and then returns if it isnt
Shader *wondererShaderGet(const char* vert_path, const char *frag_path);
void wondererShaderDestroy(Shader* shader);

#endif
