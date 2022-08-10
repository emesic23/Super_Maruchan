#ifndef __CONTROLS_H__
#define __CONTROLS_H__

#include <assert.h>
#include <stdlib.h>

#include "list.h"
#include "vector.h"
#include "scene.h"
#include "sprite.h"
#include "sdl_wrapper.h"
#include "body.h"

void player_idle(body_t* body);

// Jump action for player, only lets them jump once until returning to 
//ground again
void player_jump(body_t *body);
// Downpound action for player
void player_downpound(body_t* body);

// Moves player in dirction at constant speed
void player_move(body_t *body, double direction);

// Player releases a fireball
void player_fireball(scene_t *scene, body_t* body);

// Action to be commited upon key press.
void on_key(char key, key_event_type_t type, double held_time, scene_t *scene);

// Keys for when using the menu
void menu_on_key(char key, key_event_type_t type, double held_time, scene_t *scene);

void multiplayer_on_key(char key, key_event_type_t type, double held_time, scene_t *scene);

// Keys for when making a yes or no question
void binary_on_key(char key, key_event_type_t type, double held_time, scene_t *scene);

#endif 
