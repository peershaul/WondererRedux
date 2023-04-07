#include "../include/engine.h"

void wondererEngineInit(){
	wondererImguiInit();
	wondererBufferLoaderInit();
	wondererLayoutLoaderInit();
	wondererShaderLoaderInit();
	wondererTextureLoaderInit();
   wondererFrameBufferLoaderInit();
   wondererComputeShaderLoaderInit();
}

void wondererEngineDestroy(){
   wondererComputeShaderLoaderTerminate();
	wondererImguiDestroy();
   wondererFrameBufferLoaderTerminate();
	wondererBufferLoaderTerminate();
	wondererLayoutLoaderTerminate();
	wondererShaderLoaderTerminate();
	wondererTextureLoaderTerminate();
}
