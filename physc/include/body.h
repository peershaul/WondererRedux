#ifndef PHYSC_BODY_H
#define PHYSC_BODY_H

#include <cglm/cglm.h>
#include <stdint.h>
#include "../../wonderer/include/mem_utils.h" // need to seperate from the game engine

typedef struct Body{
	float mass;
	vec2 position, velocity;
	dynamic_arr forces; // void (**forces)(float, struct Body*, struct Body*, vec2);
} Body;

// Defining a Force type
// float: dt,
// Body*: parent,
// Body*: another body
// vec2: result
typedef void (*Force)(float, Body*, Body*, vec2); 

void physcBodyAddToSim(Body *body);
void physcBodyRemoveFromSim(Body *body);
void physcBodyFree(Body *body);
void physcBodySetForces(Body *body, Force *forces, uint16_t force_count); 

void physcBodiesSimStep(float dt);
void physcBodiesDestroySim();

#endif 
