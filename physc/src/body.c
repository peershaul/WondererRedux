#include "../include/body.h"

#include <malloc.h>
#include <stdio.h>
#include <memory.h>

struct BodySubscriber{
	Body *body;
	LinkedNode node;
};

static struct BodySubscriber *first = NULL, *last = NULL;
static uint16_t bodyCount = 0;

void physcBodyAddToSim(Body *body){
	struct BodySubscriber *subscriber = malloc(sizeof(struct BodySubscriber)); 	
	if(first == NULL)
		first = last = subscriber;	
	else {
		last->node.next = &subscriber->node;
		last = subscriber;
	}
	subscriber->node.parent = subscriber;
	subscriber->node.next = NULL;
	subscriber->body = body;
	bodyCount++;
}

void physcBodyRemoveFromSim(Body *body){
	struct BodySubscriber *curr = first, *prev = NULL;
	while(curr != NULL){
		if(curr->body == body){
			if(prev != NULL) prev->node.next = curr->node.next;
			if(last->body == body) last = prev;
			curr->body = NULL;
			free(curr);
			bodyCount--;
			break;
		}
		prev = curr;
		curr = curr->node.next->parent;
	}
}

void physcBodyFree(Body *body){
	physcBodyRemoveFromSim(body);
	wondererDynamicArrayFree(&body->forces);
}

void physcBodySetForces(Body *body, Force *forces, uint16_t force_count){
	if(body->forces.length == 0)
		wondererDynamicArrayInitialize(force_count, sizeof(Force), &body->forces);
	else if(force_count > body->forces.length)
		wondererDynamicArrayIncreaseSize(force_count, &body->forces);

	Force *arr = body->forces.arr;
	memcpy(arr, forces, sizeof(Force) * force_count);
	body->forces.last = force_count;
	if(force_count < body->forces.length) 
		memset(&arr[force_count], 0, (body->forces.length - force_count) * sizeof(Force)); 
}

void physcBodiesSimStep(float dt){
	vec2 next_positions[bodyCount], next_velocities[bodyCount];
	uint16_t i = 0;

	// We are calculating the force on this body
	struct BodySubscriber *main_body = first; 
	while(main_body != NULL){
		vec2 acceleration = {};
		struct BodySubscriber *sec_body = first;

		while(sec_body != NULL){
			if(sec_body != main_body && sec_body->body->forces.length > 0){
				Force *forces = sec_body->body->forces.arr;		
				for(int j = 0; j < sec_body->body->forces.last; j++){
					if(forces[j] == NULL) continue;
					vec2 result = {};
					(*forces[j])(dt, main_body->body, sec_body->body, result);
					glm_vec2_add(acceleration, result, acceleration);
				}
			}
			LinkedNode *sec_node = sec_body->node.next;
			sec_body = sec_node == NULL? NULL : sec_node->parent;
		}

		vec2 slot;
		glm_vec2_scale(acceleration, dt, slot);
		glm_vec2_add(main_body->body->velocity, slot, next_velocities[i]);

		glm_vec2_scale(next_velocities[i], dt, slot);
		glm_vec2_add(main_body->body->position, slot, next_positions[i]);

		LinkedNode *main_node = main_body->node.next;
		main_body = main_node == NULL? NULL : main_node->parent;
		i++;
	}

	i = 0;	
	main_body = first;
	while(main_body != NULL){
		glm_vec2_copy(next_positions[i], main_body->body->position);
		glm_vec2_copy(next_velocities[i], main_body->body->velocity);
		i++;
		LinkedNode *main_node = main_body->node.next;
		main_body = main_node == NULL? NULL : main_node->parent;
	}
}

void physcBodiesDestroySim(){
	struct BodySubscriber *curr = first, *slot = curr;
	while(curr != NULL){
		physcBodyFree(curr->body);
		curr = curr->node.next->parent;	
		free(slot);
		slot = curr;
	}
}
