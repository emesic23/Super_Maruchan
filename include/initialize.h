#ifndef __INITIALIZE_H__
#define __INITIALIZE_H__

#include "sprite.h"
#include "body.h"
#include "scene.h"
#include "color.h"
#include "shape.h"
#include "sprite.h"
#include "collision.h"
#include "forces.h"
#include "gameplay.h"


#include <math.h>

// Initializes and adds types of sprites as bodies in the inputted scene.

void add_player(scene_t *scene, vector_t position, subrole_t subrole);
void add_enemy(scene_t *scene, vector_t position, subrole_t subrole);
void add_fireball(scene_t *scene, vector_t position, vector_t velocity);
void add_token(scene_t *scene, vector_t position);
void add_powerup(scene_t *scene, vector_t position, subrole_t subrole);
void add_platform(scene_t *scene, vector_t position, vector_t size, subrole_t subrole, void* tunnel);
void add_background(scene_t *scene, vector_t size, subrole_t subrole);
void add_platform_corners(scene_t *scene, vector_t p1, vector_t p2, subrole_t subrole, void* tunnel);

// Since firball is added mid game all its interactions need to be set when its added
void initialize_fireball(scene_t *scene, body_t *fireball);

// Initializes all necessary interactions for all bodies in scene. This should
// be called only after all bodies have been added.
void add_interactions(scene_t *scene);

// Initializes actions between the platform and a role
void platform_roles_init_actions(scene_t *scene, body_t *body);
// Initialzies actions between enemies
void enemy_enemy_init_actions(scene_t* scene, list_t* list_bodies, body_t* body, size_t size, size_t curr_index);


#endif
