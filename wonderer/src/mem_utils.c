#include "../include/mem_utils.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void wondererDynamicArrayInitialize(uint16_t initial_length, uint16_t elem_size, dynamic_arr *arr){
  arr->arr = calloc(initial_length, elem_size);
  arr->last = 0;
  arr->length = initial_length;
  arr->elem_size = elem_size;
}

bool wondererDynamicArrayIncreaseSize(uint16_t new_length, dynamic_arr *arr) {
  void* new_ptr = realloc(arr->arr, new_length * arr->elem_size);
  
  if(new_ptr != NULL){
    arr->arr = new_ptr;
    arr->length = new_length;
    return false;
  }
  
  new_ptr = malloc(new_length * arr->elem_size);

  memset((arr->arr + arr->length * arr->elem_size), 0,
	 (new_length - arr->length) * arr->elem_size);  

  memcpy(new_ptr, arr->arr, arr->length * arr->elem_size);
  free(arr->arr);

  arr->arr = new_ptr;
  arr->length = new_length;
  return true;
}

void wondererDynamicArrayFree(dynamic_arr *arr){
  free(arr->arr);
}
