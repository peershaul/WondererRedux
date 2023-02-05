#ifndef WONDERER_MULTI_MEM_UTILS_H
#define WONDERER_MULTI_MEM_UTILS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint16_t last, length, elem_size;
  void* arr;
} dynamic_arr;

typedef struct LinkedNode{
	void *parent;
	struct LinkedNode *next;
} LinkedNode;

void wondererDynamicArrayInitialize(uint16_t initial_length, uint16_t elem_size, dynamic_arr *arr);

bool wondererDynamicArrayIncreaseSize(uint16_t new_length, dynamic_arr *arr);

void wondererDynamicArrayFree(dynamic_arr* arr);
#endif
