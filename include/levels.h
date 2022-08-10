#ifndef __LEVELS_H__
#define __LEVELS_H__

#include "scene.h"
#include "sdl_wrapper.h"
#include "scene.h"
#include "initialize.h"
#include "controls.h"

#include <SDL2/SDL.h>

// A function which adds some forces or impulses to bodies, e.g. from collisions, gravity, or spring forces.
typedef void (*level_creator_t)(scene_t* scene);

// Level selector
scene_t* level_select(double choice, bool multiplayer);

// Runs desired level of the game.
scene_t* level_load(level_creator_t level, bool multiplayer);

// Loads ingame sublevel caused by using a tunnel
scene_t* sublevel_load(bool multiplayer);

// Sets up the borders for the level
void level_borders(scene_t* scene, vector_t max);

// Creates scene for each level
void world1_level1(scene_t* scene);
void world1_level2(scene_t* scene);
void world1_level2_sublevel1(scene_t* scene);
void world1_level3(scene_t* scene);
void world1_level3_sublevel1(scene_t* scene);


#endif
