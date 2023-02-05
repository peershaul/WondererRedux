#include <GL/glew.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include "../include/mem_utils.h"
#include "../include/logger.h"
#include "../../vendor/stb/stb_image.h"

typedef struct {
	GLuint id;
  GLenum color_type;
  uint16_t width, height;
	char* filepath;
} Texture;

#define WONDERER_MULTI_TEXTURE_SRC
#include "../include/texture.h"

static dynamic_arr texture_pool;
static Texture **bound;
static GLint texture_bind_count = 0;
static uint8_t curr_slot = 0;

void wondererTextureLoaderInit() {
  wondererDynamicArrayInitialize(2, sizeof(Texture), &texture_pool);
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_bind_count);
  bound = calloc(texture_bind_count, sizeof(Texture*));
  WondererLog("Your GPU supports %d textures", texture_bind_count);
  glActiveTexture(GL_TEXTURE0);
}

void wondererTextureLoaderTerminate() {
  free(bound);

  Texture *arr = texture_pool.arr;
  for(uint16_t i = 0; i < texture_pool.last; i++)
    glDeleteTextures(1, &arr[i].id);

  for(uint8_t i = 0; i < texture_bind_count; i++){
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

}

Texture create_texture() {
   GLuint id;
   glGenTextures(1, &id);
   
   Texture* last_bound = wondererTextureGetBound(curr_slot);
   
   glBindTexture(GL_TEXTURE_2D, id);
   
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  wondererTextureBind(curr_slot, last_bound);

  return (Texture){ .id = id };
}

Texture *wondererTextureCreate() {
  Texture *arr = texture_pool.arr;
  int32_t found = -1;


  for(uint16_t i = 0; i < texture_pool.last; i++)
    if(arr[i].id == 0){
      found = i;
      break;
    }

  if(found != -1){
    arr[found] = create_texture();
    return &arr[found];
  }

  if(texture_pool.last == texture_pool.length){
    wondererDynamicArrayIncreaseSize(2 * texture_pool.length, &texture_pool);
    arr = texture_pool.arr;
  }

  arr[texture_pool.last] = create_texture();
  return &arr[texture_pool.last++];
}

void wondererTextureDestroy(Texture *tex) {
  for(uint8_t i = 0; i < texture_bind_count; i++)
    if(bound[i] == tex){
      bound[i] = NULL;
    }

	if(tex->filepath != NULL) free(tex->filepath);
  glDeleteTextures(1, &tex->id);
  *tex = (Texture){};
}


void wondererTextureBind(uint8_t slot, Texture *tex) {

  if(slot >= texture_bind_count){
    WondererError("Cant bind texture slot %d, GPU doesnt support that");
    return;
  }

  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, tex == NULL? 0 : tex->id);

  curr_slot = slot;

  bound[slot] = tex;
}

Texture *wondererTextureGetBound(uint8_t slot) {
  if(slot >= texture_bind_count) return NULL;
  return bound[slot];
}

void wondererTextureUploadTexImage(Texture* tex, const char *path) {
  int width, height, nrChannels; 

	stbi_set_flip_vertically_on_load(true);

  uint8_t *data = stbi_load(path, &width, &height, &nrChannels, 0);
  GLenum format = nrChannels == 3? GL_RGB : GL_RGBA;

  Texture *last_bound = wondererTextureGetBound(curr_slot);
  wondererTextureBind(curr_slot, tex);

  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

	if(tex->filepath != NULL) free(tex->filepath);
	tex->filepath = malloc(strlen(path) * sizeof(char));
	strcpy(tex->filepath, path);

  wondererTextureBind(curr_slot, last_bound);
}

GLuint wondererTextureGetID(Texture *tex){
	return tex->id; 
}

Texture *wondererTextureGet(const char* filepath){

	if(texture_pool.last != 0){
		Texture *textures = texture_pool.arr;
		for(int i = 0; i < texture_pool.last; i++)
			if(strcmp(textures[i].filepath, filepath) == 0)
				return &textures[i];
	}

	Texture *tex = wondererTextureCreate();
	wondererTextureUploadTexImage(tex, filepath);
	return tex;
}
