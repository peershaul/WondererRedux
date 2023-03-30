#include <math.h>
#define WONDERER_MULTI_FRAMEBUFFERS_SRC

#include <GL/glew.h>
#include <stdint.h> 
#include <cglm/cglm.h>
#include <malloc.h>

#include "../include/texture.h"
#include "../include/mem_utils.h"
#include "../include/logger.h"

typedef struct {
   GLuint fb_id, rb_id;
   uint16_t height, width;
   Texture *tex;
   vec3 clear_color;
} FrameBuffer;

#include "../include/framebuffer.h"

static dynamic_arr framebuffer_pool = {}; 
static FrameBuffer *bound = NULL;

static FrameBuffer create_framebuffer(uint16_t width, uint16_t height){

   GLenum parameters[] = {GL_NEAREST, GL_NEAREST, 
                          GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE};

   FrameBuffer *last_bound = bound;

   GLuint fb_id;
   glGenFramebuffers(1, &fb_id);
   glBindFramebuffer(GL_FRAMEBUFFER, fb_id); 

   Texture *tex = wondererTextureCreateWithParameters(parameters);
   wondererTextureSetImageParams(tex, width, height, GL_RGB);

   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 
                          wondererTextureGetID(tex), 0);

   GLuint rb_id;
   glGenRenderbuffers(1, &rb_id);
   glBindRenderbuffer(GL_RENDERBUFFER, rb_id); 
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, 
                             GL_RENDERBUFFER, rb_id); 

   wondererFrameBufferBind(last_bound);

   GLuint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if(status != GL_FRAMEBUFFER_COMPLETE){
      WondererError("Frame buffer error: %X (%d)", status, status);
      return (FrameBuffer){0};
   }

   return (FrameBuffer) {fb_id, rb_id, height, width, tex};
}

static int search_for_available_space(){
   FrameBuffer *arr = framebuffer_pool.arr;

   for(int i = 0; i < framebuffer_pool.last; i++)
      if(arr[i].fb_id == 0)
         return i;

   return -1;
}

void wondererFrameBufferLoaderInit(){
   wondererDynamicArrayInitialize(2, sizeof(FrameBuffer), &framebuffer_pool);
}

void wondererFrameBufferLoaderTerminate(){

   FrameBuffer *arr = framebuffer_pool.arr;  

   for(int i = 0; i < framebuffer_pool.last; i++){
      if(arr[i].fb_id == 0) continue;
      wondererFrameBufferDestroy(&arr[i]); 
   }

   wondererDynamicArrayFree(&framebuffer_pool);
}

FrameBuffer *wondererFrameBufferCreate(uint16_t width, uint16_t height){

   FrameBuffer fb = create_framebuffer(width, height); 
   if(fb.fb_id == 0) return NULL;

   int found_index = search_for_available_space();

   if(found_index == -1){
      if(framebuffer_pool.last >= framebuffer_pool.length)
         wondererDynamicArrayIncreaseSize(2 * framebuffer_pool.length, &framebuffer_pool);

      FrameBuffer *arr = framebuffer_pool.arr;

      arr[framebuffer_pool.last] = fb;
      return &arr[framebuffer_pool.last++];
   }

   else {
      FrameBuffer *arr = framebuffer_pool.arr;
      arr[found_index] = create_framebuffer(width, height);
      return &arr[found_index];
   }
}

void wondererFrameBufferDestroy(FrameBuffer *fb){
   glDeleteFramebuffers(1, &fb->fb_id);
   glDeleteRenderbuffers(1, &fb->rb_id);

   wondererTextureDestroy(fb->tex);
}

FrameBuffer *wondererFrameBufferGetBound(){ return bound; }

void wondererFrameBufferBind(FrameBuffer *fb){
   if(fb == NULL){
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glDisable(GL_DEPTH_TEST);
   }
   else{
      glBindFramebuffer(GL_FRAMEBUFFER, fb->fb_id);
      glClearColor(fb->clear_color[0], fb->clear_color[1], fb->clear_color[2], 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
   }
}

GLuint wondererFrameBufferGetFBID(FrameBuffer *fb) { return fb->fb_id; }
GLuint wondererFrameBufferGetRBID(FrameBuffer *fb) { return fb->rb_id; }
Texture *wondererFrameBufferGetTexture(FrameBuffer *fb){ return fb->tex; }

void wondererFrameBufferSetClearColor(FrameBuffer *fb, vec3 color){ glm_vec3_copy(color, fb->clear_color); }

