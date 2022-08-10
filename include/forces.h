#ifndef __FORCES_H__
#define __FORCES_H__

#include "stdio.h"
#include "list.h"
#include "body.h"
#include "collision.h"
#include "interaction.h"
#include "scene.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <assert.h>


// Collision handling  -------------------------------------------------------------


// Adds a force creator to a scene that calls a given collision handler function each time two bodies collide.
// This generalizes create_destructive_collision() from last week, allowing different things to happen on a collision.
// It should only be called once while the bodies are still colliding.
void create_collision(scene_t *scene, body_t *body1, body_t *body2, force_creator_t handler, void *aux, free_func_t freer);


// Adds a collision_handler_t to a scene which applies an elastic collision interaction between two bodies.
void create_physics_collision(scene_t* scene, body_t* body1, body_t* body2, double e);


// Adds a collision_handler_t to a scene which applies a destructive collision interaction between two bodies.
void create_destructive_collision(scene_t* scene, body_t* body1, body_t* body2);

    
// force_creator_t functions for implementing collisions --------------------------

// Implements a 'destructive collision' interaction between two bodies.
void destructive_collision(interaction_t* interaction);


// Implements a physics collision interaction upon contact. The stuff to
// make sure multiple impulses aren't added is dealt with in body.c to avoid
// having to write new accessor functions.
void physics_collision(interaction_t* interaction);


// Force handling ------------------------------------------------------------

// Adds a force creator to a scene that applies gravity between two bodies.
void create_newtonian_gravity(scene_t *scene, double G, body_t *body1, body_t *body2);


// Adds a force creator to a scene that acts like a spring between two bodies.
void create_spring(scene_t *scene, double k, body_t *body1, body_t *body2);


// Adds a force creator to a scene that applies a drag force on a body.
void create_drag(scene_t *scene, double gamma, body_t *body);



// force_creator_t functions which implement each force  ---------------------------------

void newtonian_gravity(interaction_t *interaction);

void spring(interaction_t *interaction);

void drag(interaction_t *interaction);

void destructive_collision(interaction_t *interaction);

#endif // #ifndef __FORCES_H__
