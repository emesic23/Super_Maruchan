#ifndef __SPRITE_H__
#define __SPRITE_H__

#include <assert.h>
#include <stdlib.h>

#include "list.h"
#include "vector.h"

extern const size_t NUM_ROLES;
extern const size_t NUM_LEVELS;
extern const size_t NUM_CUTSCENES;

extern const vector_t PLAYER_SHEET;
extern const vector_t ENEMY_SHEET;
extern const vector_t PLATFORM_SHEET;


typedef struct sprite_state
{
    vector_t start;
    // Number of frames associated with a state.
    size_t frames;
    vector_t scale;
    vector_t adjust;

} sprite_state_t;

// Different sprite_states (these shouldn't be in the constants.h file).
extern const sprite_state_t PLAYER_IDLE_RIGHT;
extern const sprite_state_t PLAYER_IDLE_LEFT;
extern const sprite_state_t PLAYER_RUNNING_RIGHT;
extern const sprite_state_t PLAYER_RUNNING_LEFT;
extern const sprite_state_t PLAYER_JUMPING_RIGHT;
extern const sprite_state_t PLAYER_JUMPING_LEFT;
extern const sprite_state_t PLAYER_GROUNDPOUND_LEFT;
extern const sprite_state_t PLAYER_GROUNDPOUND_RIGHT;

extern const double STAR_MODE;
extern const double FIRE_MODE;

extern const sprite_state_t GOOMBA_WALKING;
extern const sprite_state_t GOOMBA_SQUASHED;
extern const sprite_state_t KOOPA_LEFT;
extern const sprite_state_t KOOPA_RIGHT;
extern const sprite_state_t KOOPA_SHELL;

extern const sprite_state_t NO_STATE;
extern const sprite_state_t TOKEN_STATE;
extern const sprite_state_t HEALTH_STATE;
extern const sprite_state_t BACKGROUND_STATE;

extern const sprite_state_t BLOCK_ITEM;
extern const sprite_state_t BLOCK_TOKEN; 
extern const sprite_state_t BLOCK_HEALTH;
extern const sprite_state_t BLOCK_STAR;
extern const sprite_state_t BLOCK_FIRE;
typedef enum role
{
    BACKGROUND, // 0
    PLATFORM, // 1
    POWERUP, // 2
    TOKEN, // 3
    ENEMY, // 4
    PLAYER, // 5
} role_t;

// Further species type in each respective role
// We could consider passing in void pointer...and then make a struct for each role...idk
typedef enum subrole_t
{
    // Enemy types
    GOOMBA,
    KOOPA,
    // Player 1 or 2
    PLAYER1,
    PLAYER2,
    // NO SUBROLE
    NO_SUBROLE,
    PLACEHOLDER, // need this to make numbering work for texture loading
    // Levels -- important that levels start here because texture loading is based on it
    LEVEL1,
    LEVEL2,
        LEVEL2_1,
    LEVEL3,
        LEVEL3_1,
    // Scenes for opening 
    CUTSCENE1,
    CUTSCENE2,
    CUTSCENE3,
    CUTSCENE4,
    CUTSCENE5,
    CUTSCENE6,
    CUTSCENE7,
    // Scenes for Directions
    CUTSCENE8,
    // Powerup type
    HEALTH_POWERUP,
    STAR_POWERUP,
    FIRE_POWERUP,
        // used during fire powerup
        FIREBALL,
    TOKEN_POWERUP,

    // Platform type
    REGULAR_BLOCK,
    ITEM_BLOCK,
    DEATH_BLOCK,
    TUNNEL_BLOCK,
    INVISIBLE_BLOCK,

} subrole_t;

// Provides information about what a sprite is in contact with.
typedef struct contact
{
    bool left;
    bool right;
    bool above;
    bool below;
} contact_t;


typedef struct sprite sprite_t;

// Returns whether or not two sprite states are equal.
bool sprite_state_equal(sprite_t* s1, sprite_state_t s2);

// Selects state depending on role and subrole
sprite_state_t state_select(role_t role, subrole_t subrole);

// Initializes a generic sprite.
sprite_t* sprite_init(role_t role, subrole_t subrole, size_t health, bool animated);

// Initialize a sprite struct with info.
sprite_t *sprite_init_with_info(role_t role, subrole_t subrole, size_t health, bool animated, void* info, free_func_t info_freer);

// Free sprite
void sprite_free(sprite_t* sprite);

// Increase token score by one.
void sprite_add_token(sprite_t *sprite);

// Handles the sprite animation. We can add to this later for more
// functionality.
void sprite_tick(sprite_t* sprite, double dt);


// Switches to the next frame in the sprite sheet.
void sprite_next_frame(sprite_t* sprite);

// Increase kill by one.
void sprite_add_kills(sprite_t *player);

// Decrementes health by one.
void sprite_lower_health(sprite_t *player);

// Returns the contact information of the sprite.
contact_t sprite_contact(sprite_t *sprite);

// Returns role.
role_t sprite_role(sprite_t *sprite);

// Gets the sprite's role.
subrole_t sprite_subrole(sprite_t *player);

// Change the value of the contact fields.
void sprite_set_contact(sprite_t* player, contact_t contact);

// Applies a powerup.
void sprite_activate_powerup(sprite_t *sprite, subrole_t powerup);

// Gets the current frame.
vector_t sprite_current_frame(sprite_t* sprite);

// Sets current frame....this is used to change graphics back to normal mode
void sprite_set_current_frame(sprite_t* sprite, double row);

// Immobilize or restore motion capability to a sprite.
void sprite_immobilized_tick(sprite_t* sprite);

// Get whether or not a sprite is immobilized.
double sprite_is_immobilized(sprite_t* sprite);

// Get the current state of the sprite. 
sprite_state_t sprite_get_state(sprite_t* sprite);

// Returns sprite info field
void* sprite_get_info(sprite_t* sprite);

// Sets info for sprite
void sprite_set_info(sprite_t* sprite, void* info);

// Change the state of a sprite.
void sprite_set_state(sprite_t* sprite, sprite_state_t state);

size_t sprite_health(sprite_t *sprite);

size_t sprite_kills(sprite_t *sprite);

size_t sprite_tokens(sprite_t *sprite);
// updates kills and token of sprite
void sprite_update(sprite_t *sprite, size_t kills, size_t tokens, size_t health);

#endif
