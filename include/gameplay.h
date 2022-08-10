#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include <stdlib.h>
#include <stdio.h>

#include "gameplay.h"
#include "interaction.h"
#include "scene.h"
#include "collision.h"
#include "body.h"
#include "sprite.h"
#include "collision.h"

typedef struct tunnel tunnel_t;
typedef struct item item_t;
// Creates an action between two bodies
void gameplay_create(scene_t *scene, body_t *body1, body_t *body2, force_creator_t handler, void *aux, free_func_t freer);

// Gameplay operations ------------------------------------------------------------------------------------------------
// ORDER: GENERAL -> TOKEN -> ENEMY -> PLATFORM -> PLAYER

// General gameplay -------------------------------------------------

// Player mechanics such as gravity
void gameplay_gravity(interaction_t *interaction);

// Enemy mechanics such as patrol radius
void gameplay_patrol(interaction_t *interaction);

// Applies bot mechnics to enemies and powerups.... Could later change this to work with gameplay_Create
void create_bot_mechanics(scene_t* scene, body_t* bot, vector_t speed);

// applies elastic collisions effect
void elastic_collisions(body_t *body1, body_t *body2, collision_info_t collision, double k, bool apply1, bool apply2);

// Token gameplay ------------------------------------------

// interaction with player and token
void gameplay_token(interaction_t* interaction);

// Enemy gameplay ------------------------------------------

// interactions with player and enemy
void gameplay_goomba(interaction_t* interaction);
void gameplay_koopa(interaction_t* interaction);
void gameplay_koopa_enemy(interaction_t* interaction);

// Gameplay for platforms --------------------------------------------------------------------------------------------------------------

// Interactions of player with environment
void gameplay_regular_block(interaction_t* interaction);
// Player dies when they touch this block
void gameplay_death_block(interaction_t* interaction);
// Trasnports player to a sublevel
void gameplay_tunnel_block(interaction_t* interaction);
    // Both used in tunnel to select approtiate level
    void sublevel_select(double sublevel);
    double sublevel_run(); // Called in levels.c
    // Returns tunnel struct
    tunnel_t* tunnel_init(bool top, bool left, bool bottom, bool right, double sublevel, vector_t exit);
// Causes a powerup item to pop up
void gameplay_item_block(interaction_t* interaction);
// Invisible block that changes enemy direction
void gameplay_invisible_block(interaction_t* interaction);

// Gameplay for powerups --------------------------------------------
// Players because invicible for a few seconds
void gameplay_star_powerup(interaction_t* interaction);

// Fireball interaction, fireball kills all enemies and only lasts 3 bounces 
void gameplay_fireball(interaction_t* interaction);

// PLAYER gameplay --------------------------------------------------------------------------------------------------

// Applies basic bot patrol where power/enemy move back and forth certain radius from starting point
void gameplay_player(interaction_t *interaction);

#endif
