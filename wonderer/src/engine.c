#include "../include/engine.h"

void wondererEngineInit(){
	wondererImguiInit();
	wondererBufferLoaderInit();
	wondererLayoutLoaderInit();
	wondererShaderLoaderInit();
	wondererTextureLoaderInit();
}

void wondererEngineDestroy(){
	wondererImguiDestroy();
	wondererBufferLoaderTerminate();
	wondererLayoutLoaderTerminate();
	wondererShaderLoaderTerminate();
	wondererTextureLoaderTerminate();
}
