#include "sprite.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL_mixer.h>

// #include "sequences.h"

const size_t FRAME_RATE = 20;

// Sprite sheet dimensions, i.e. the number ofr "rows" and "columns."
const vector_t PLAYER_SHEET = {12, 12};
const vector_t ENEMY_SHEET = {9, 15};
const vector_t PLATFORM_SHEET = {1, 5};

// Audio files
extern Mix_Chunk *gToken;

// Number of role_t.
const size_t NUM_ROLES = 6;
const size_t NUM_LEVELS = 5;
const size_t NUM_CUTSCENES = 8;

// Scale for dest_rect in sdl_draw.c
const vector_t PLAYER_SCALE = {4, 4};
const vector_t ENEMY_SCALE = {3, 3};

const vector_t PLAYER_ADJUST = {-40, -15};

const double IMMOBILIZATION_TICK = 0.2;

// Different sprite_states.
const sprite_state_t PLAYER_IDLE_RIGHT = {(vector_t){0, 6}, 1, (vector_t) {1.5,1.5}, (vector_t) {-10, -5}};
const sprite_state_t PLAYER_IDLE_LEFT = {(vector_t){0, 5}, 1, (vector_t) {1.5,1.5}, (vector_t) {-10, -5}};
const sprite_state_t PLAYER_RUNNING_RIGHT = {(vector_t){0, 7}, 3, (vector_t) {1.5,1.5}, (vector_t) {-10, -5}};
const sprite_state_t PLAYER_RUNNING_LEFT = {(vector_t){0, 2}, 3, (vector_t) {1.5,1.5}, (vector_t) {-10, -5}};
const sprite_state_t PLAYER_JUMPING_RIGHT = {(vector_t){0, 11}, 1, (vector_t) {1.5,1.5}, (vector_t) {-10, -5}};
const sprite_state_t PLAYER_JUMPING_LEFT = {(vector_t){0, 0}, 1, (vector_t) {1.5,1.5}, (vector_t) {-10, -5}};
const sprite_state_t PLAYER_GROUNDPOUND_LEFT = {(vector_t){1, 0}, 1, (vector_t) {1.5,1.5}, (vector_t) {-10, -5}};
const sprite_state_t PLAYER_GROUNDPOUND_RIGHT = {(vector_t){1, 11}, 1, (vector_t) {1.5,1.5}, (vector_t) {-10, -5}};

// Different sprite_states.
const sprite_state_t PLAYER2_GROUNDPOUND_LEFT = {(vector_t){7, 0}, 1, (vector_t) {3,3}, (vector_t) {-35, -35}};
const sprite_state_t PLAYER2_GROUNDPOUND_RIGHT = {(vector_t){7, 11}, 1, (vector_t) {3,3}, (vector_t) {-35, -35}};
// Enemy related states
const sprite_state_t GOOMBA_WALKING = {(vector_t){0, 0}, 2, (vector_t) {1,1}, (vector_t) {10, 5}};
const sprite_state_t GOOMBA_SQUASHED = {(vector_t){0, 2}, 1, (vector_t) {1,1}, (vector_t) {0, 10}};
const sprite_state_t KOOPA_LEFT = {(vector_t){0, 5}, 2, (vector_t) {1,1}, (vector_t) {0, 5}};
const sprite_state_t KOOPA_RIGHT = {(vector_t){0, 7}, 2, (vector_t) {1,1}, (vector_t) {0, 5}};
const sprite_state_t KOOPA_SHELL = {(vector_t){0, 12}, 1, (vector_t) {1,1}, (vector_t) {-10, 10}};
// Token States
const sprite_state_t TOKEN_STATE = {(vector_t){0, 0}, 1, (vector_t) {0.2,0.2}, (vector_t) {70, 90}};
// Powerup states
const sprite_state_t HEALTH_STATE = {(vector_t){0, 0}, 1, (vector_t) {0.03,0.03}, (vector_t) {225, 220}};
const sprite_state_t FIREBALL_STATE = {(vector_t){0, 0}, 1, (vector_t) {0.05,0.05}, (vector_t) {122, 120}};
const sprite_state_t NO_STATE = {(vector_t){0, 0}, 1, (vector_t) {0.2,0.2}, (vector_t) {0, 0}};

// ITEM_BLOCK STATES
const sprite_state_t BLOCK_ITEM = {(vector_t){0, 0}, 1, (vector_t) {2.4,2.2}, (vector_t) {-17, -17}};
const sprite_state_t BLOCK_TOKEN = {(vector_t){0, 1}, 1, (vector_t) {2.4,2.2}, (vector_t) {-17, -17}};
const sprite_state_t BLOCK_HEALTH = {(vector_t){0, 4}, 1, (vector_t) {2.4,2.2}, (vector_t) {-17, -17}};
const sprite_state_t BLOCK_STAR = {(vector_t){0, 2}, 1, (vector_t) {2.4,2.2}, (vector_t) {-17, -17}};
const sprite_state_t BLOCK_FIRE = {(vector_t){0, 3}, 1, (vector_t) {2.4,2.2}, (vector_t) {-17, -17}};

// How much animation row is adjusted by for each powerup mode that requires change in animation
const double STAR_MODE = 2;
const double FIRE_MODE = 4;


const sprite_state_t BACKGROUND_STATE = {(vector_t){0, 0}, 1, (vector_t) {1, 1}, (vector_t) {0, 0}};
const sprite_state_t White_BACKGROUND_STATE = {(vector_t){0, 0}, 1, (vector_t) {5, 5}, (vector_t) {-1000, 0}};

typedef struct sprite
{

    role_t role;
    subrole_t subrole;
    // This will allow us to check if a sprite is animated before trying to
    // update it.
    bool animated;

    // Any additional information associated with the body
    void *info;
    free_func_t info_freer;

    size_t health;
    size_t tokens;
    size_t kills;

    // If contact something to left, right, above, below
    contact_t contact;
    // Allows us to prevent the player from moving the sprite if we need to.
    double immobilized;

    // Stuff for animation --------------------------------------------

    double time_since_last_frame;
    vector_t current_frame;
    sprite_state_t state;

} sprite_t;

// Returns whether or not two sprite states are equal.
bool sprite_state_equal(sprite_t* s1, sprite_state_t s2)
{
    return (s1 -> state.start.x == s2.start.x) &&
           (s1 -> state.start.y == s2.start.y) &&
           (s1 -> state.frames == s2.frames);
}

// Selects state depending on role and subrole
sprite_state_t state_select(role_t role, subrole_t subrole)
{
    // Player states
    if (role == PLAYER) {return PLAYER_IDLE_RIGHT;}
    // Enemy states
    if (subrole == GOOMBA) {return GOOMBA_WALKING;}
    if (subrole == KOOPA) {return KOOPA_RIGHT;} // state needs to be added
    // Powerup states
    if (subrole == HEALTH_POWERUP) {return HEALTH_STATE;}
    if (subrole == STAR_POWERUP) {return NO_STATE;} // needs to be added
    if (subrole == FIRE_POWERUP) {return NO_STATE;} // needs to be added
    // Platform states
    if (subrole == FIREBALL) {return FIREBALL_STATE;}
    if (subrole == ITEM_BLOCK) {return BLOCK_ITEM;} // get their own unique graphic
    if (role == PLATFORM) {return NO_STATE;} // rest of platforms don't have graphics
    // Background and token states
    if (role == BACKGROUND) {return BACKGROUND_STATE;}
    if (role == TOKEN) {return TOKEN_STATE;}
    return NO_STATE;
}

// Initialize a generalized sprite struct.
sprite_t *sprite_init(role_t role, subrole_t subrole, size_t health, bool animated)
{
    sprite_t *sprite = malloc(sizeof(sprite_t));

    sprite->role = role;
    sprite->subrole = subrole;

    sprite->animated = animated;
    
    sprite->info = NULL;
    sprite->info_freer = free;

    sprite->health = health;
    sprite->tokens = 0;
    sprite->kills = 0;

    sprite->contact = (contact_t){false, false, false, false};
    sprite->immobilized = 0;

    sprite->time_since_last_frame = 0;
    sprite_state_t state = state_select(role, subrole);
    // Sets correct y axis for player1 vs player2 animations
    // The row or x of curr_frame is only altered when changing graphics for player
    // One example would be switching from regular graphics to fire mode graphics
    if (subrole == PLAYER1) {sprite->current_frame = (vector_t) {0, 0};}
    else if (subrole == PLAYER2) {sprite->current_frame = (vector_t) {6, 0};}
    else {sprite->current_frame = (vector_t) {state.start.x, 0};}

    // Only add x to stay in same row
    sprite->state = state;

    return sprite;
}

// Initialize a sprite struct with info.
sprite_t *sprite_init_with_info(role_t role, subrole_t subrole, size_t health, bool animated, void* info, free_func_t info_freer)
{
    sprite_t* sprite = sprite_init(role, subrole, health, animated);
    sprite -> info = info;
    sprite -> info_freer = info_freer;
    return sprite;
}

// Frees sprite
void sprite_free(sprite_t* sprite)
{
    if (sprite -> info != NULL && sprite -> info_freer != NULL)
    {
        (sprite -> info_freer)(sprite -> info);
    }
    free(sprite);
}

// Handles the sprite animation. We can add to this later for more
// functionality.
void sprite_tick(sprite_t *sprite, double dt)
{
    // Time that player is immobilized for
    if (sprite->immobilized > 2) {sprite->immobilized = 0;}
    else if (sprite->immobilized >= IMMOBILIZATION_TICK) {sprite->immobilized += dt;}

    if(sprite -> state.frames == 1) {return;}
    // and then we wouldn't need an animation field, idk if thats slower or faster

    sprite->time_since_last_frame += dt;
    // FRAME_RATE is frames per second, so its inverse is the number of seconds
    // between each frame.
    if (sprite->time_since_last_frame > (5.0 / (double)FRAME_RATE))
    {
        sprite->time_since_last_frame = 0;
        sprite_next_frame(sprite);
    }
}

// Switches to the next frame in the sprite sheet.
void sprite_next_frame(sprite_t *sprite)
{
    sprite_state_t state = sprite->state;
    // NOTE TO PIPPA: added this so it doesn't try move to animate things with one frame
    vector_t curr = sprite->current_frame;

    // Stay in the same row, but advance to the next frame in the state.
    sprite->current_frame = (vector_t){curr.x, (((int)curr.y) + 1) % state.frames};
}

// Increases the number of tokens collected by one.
void sprite_add_token(sprite_t *player)
{
    assert(sprite_role(player) == PLAYER);
    Mix_PlayChannel(-1, gToken, 0);
    player->tokens ++;
}

// Increases the number of kills by one.
void sprite_add_kills(sprite_t *player)
{
    assert(sprite_role(player) == PLAYER);
    player->kills = player->kills + 1;
}

// Decrements health by one.
void sprite_lower_health(sprite_t *player)
{
    // If in star mode they are invincible and loss no health
    // The +6 is for player 2
    if (player->current_frame.x == STAR_MODE || player->current_frame.x == STAR_MODE + 6) {return;}
    if(player->health != 0) {player->health = player->health - 1;}
    if(player->health > 100) {player->health = 0;}
    vector_t v = player->current_frame;
    if (player -> subrole == PLAYER1) {player->current_frame = (vector_t) {0, v.y};}
    if (player -> subrole == PLAYER2) {player->current_frame = (vector_t) {6, v.y};}
    
}

// Set and accesor functions  ---------------------------------------------------------

// Gets the current frame.
vector_t sprite_current_frame(sprite_t *sprite)
{
    return sprite->current_frame;
}

// Checks to see what the sprite is in contact with.
contact_t sprite_contact(sprite_t *sprite)
{
    return sprite->contact;
}

// Gets the sprite's role.
role_t sprite_role(sprite_t *player)
{
    return player->role;
}

// Gets the sprite's role.
subrole_t sprite_subrole(sprite_t *player)
{
    return player->subrole;
}

// Change the value of the contact fields
void sprite_set_contact(sprite_t *player, contact_t contact)
{
    player->contact = contact;
}

// Applies a powerup to the player sprite and changes the animation accordingly
void sprite_activate_powerup(sprite_t *player, subrole_t powerup)
{
    assert(sprite_role(player) == PLAYER);
    if (powerup == HEALTH_POWERUP)
    {
        Mix_PlayChannel(-1, gHealth, 0);
        player->health++;
    }
    if (powerup == STAR_POWERUP)
    {
        Mix_PlayChannel(-1, gPowerUp, 0);
        vector_t v = player->current_frame;
        player->current_frame = (vector_t) {v.x + STAR_MODE, v.y};
    }
    if (powerup == FIRE_POWERUP)
    {
        Mix_PlayChannel(-1, gPowerUp, 0);
        vector_t v = player->current_frame;
        player->current_frame = (vector_t) {v.x + FIRE_MODE, v.y};
    }
    if (powerup == TOKEN_POWERUP)
    {
        Mix_PlayChannel(-1, gToken, 0);
        player->tokens++; 
    }
}

// Immobilize or restore motion capability to a sprite.
void sprite_immobilized_tick(sprite_t *sprite)
{
    sprite->immobilized = sprite->immobilized + IMMOBILIZATION_TICK;
}

// Get whether or not a sprite is immobilized.
double sprite_is_immobilized(sprite_t *sprite)
{
    return sprite->immobilized;
}

// Get the current state of the sprite.
sprite_state_t sprite_get_state(sprite_t *sprite)
{
    return sprite->state;
}

// Change the state of a sprite.
void sprite_set_state(sprite_t *sprite, sprite_state_t state)
{
    // Undoes groundpount + 1 to row if previous state was groundpound
    if (sprite_state_equal(sprite, PLAYER_GROUNDPOUND_RIGHT)) {sprite->current_frame = (vector_t) {sprite->current_frame.x - 1, 0};}
    if (sprite_state_equal(sprite, PLAYER_GROUNDPOUND_LEFT)) {sprite->current_frame = (vector_t) {sprite->current_frame.x - 1, 0};}    
    // Sets the state
    sprite->state = state;
    if (sprite_state_equal(sprite, PLAYER_GROUNDPOUND_RIGHT)) {sprite->current_frame = (vector_t) {sprite->current_frame.x + 1, 0};}
    if (sprite_state_equal(sprite, PLAYER_GROUNDPOUND_LEFT)) {sprite->current_frame = (vector_t) {sprite->current_frame.x + 1, 0};}
    else if (state.frames == 1) // Only resets curr_frame to 0 if no animation
    {sprite->current_frame = (vector_t) {sprite->current_frame.x, 0};}
}

// Returns sprite info field
void* sprite_get_info(sprite_t* sprite)
{
    return sprite->info;
}

// Sets info for sprite
void sprite_set_info(sprite_t* sprite, void* info)
{
    void* old_info = sprite -> info;
    sprite -> info = info;
    if(old_info != NULL) {free(old_info);}
}

// Sets current frame....this is used to change graphics back to normal mode
void sprite_set_current_frame(sprite_t* sprite, double row)
{
    sprite -> current_frame = (vector_t) {row, sprite -> current_frame.y};  
}

// Returns the health of the sprite.
size_t sprite_health(sprite_t *sprite)
{
    return sprite->health;
}

size_t sprite_kills(sprite_t *sprite)
{
    return sprite->kills;
}

size_t sprite_tokens(sprite_t *sprite)
{
    return sprite->tokens;
}

// updates kills and token of sprite
void sprite_update(sprite_t *sprite, size_t kills, size_t tokens, size_t health)
{
    sprite -> health = health;
    sprite -> kills = kills;
    sprite -> tokens = tokens;
}