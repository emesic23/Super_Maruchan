#ifndef __SCENE_H__
#define __SCENE_H__

#include "list.h"
#include "body.h"
#include "interaction.h"
#include "sprite.h"

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// A collection of lists and force creators. The scene automatically resizes to store
// arbitrarily many lists and force creators.
typedef struct scene scene_t;


// Stores which sides of scene have bounds
typedef struct scene_bounds
{
    bool left;
    bool top;
    bool right;
    bool bottom;
    double boundaries;
} scene_bounds_t;


// Allocates memory for an empty scene.
scene_t *scene_init();


// Releases memory allocated for a given scene and all the lists and force creators it contains.
void scene_free(scene_t *scene);

// Adds a body to a scene.
void scene_add_body(scene_t *scene, body_t *body, size_t index);


// @deprecated Use body_remove() instead
// Removes and frees the body at a given index from a scene.
void scene_remove_body(scene_t *scene, size_t index, size_t role);


// Adds a force creator to a scene, to be invoked every time scene_tick() is called.
// The auxiliary value is passed to the force creator each time it is called.
// The force creator is registered with a list of bodies it applies to, so it can be
// removed when any one of the bodies is removed.
void scene_add_bodies_force_creator(scene_t *scene, force_creator_t forcer, void *aux, list_t *bodies, free_func_t freer);


// Executes a tick of a given scene over a small time interval. If any bodies are marked for removal,
// they should be removed from the scene and freed, along with any force creators acting on them.
void scene_tick(scene_t *scene, double dt);


// Remove all interactions (i.e. force interactions) associated with the inputted
// body from the scene.
void scene_remove_interactions(scene_t *scene, body_t *body);

// Accessor functions ---------------------------------------------------

// Gets the body at a given index in a scene.
list_t *scene_get_list(scene_t *scene, size_t role);

// Returns number of lists in scene
size_t scene_size(scene_t *scene);

// Gets the body at a given index in a scene.
body_t *scene_get_body(scene_t *scene, role_t role, size_t index);



#endif // #ifndef __SCENE_H__
