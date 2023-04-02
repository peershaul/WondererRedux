#include "../include/engine.h"

void wondererEngineInit(){
	wondererImguiInit();
	wondererBufferLoaderInit();
	wondererLayoutLoaderInit();
	wondererShaderLoaderInit();
	wondererTextureLoaderInit();
   wondererFrameBufferLoaderInit();
}

void wondererEngineDestroy(){
	wondererImguiDestroy();
   wondererFrameBufferLoaderTerminate();
	wondererBufferLoaderTerminate();
	wondererLayoutLoaderTerminate();
	wondererShaderLoaderTerminate();
	wondererTextureLoaderTerminate();
}
