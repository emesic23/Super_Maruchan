#include "controls.h"
#include "menu.h"
#include "initialize.h"
#include "sdl_wrapper.h"
#include <SDL2/SDL_mixer.h>

// Impluses for player movement
const vector_t JUMP_IMPULSE = {0, 2000};
const vector_t DOWN_IMPULSE = {0, 1000};
// Directions and vellocity for arrows keys/awd
const double RIGHT = 800;
const double LEFT = -800;

extern const vector_t FIREBALL_RIGHT;
extern const vector_t FIREBALL_LEFT;
extern Mix_Music* gMenuMusic;
extern Mix_Chunk* gJump;

// Puts the player into an idle state.
void player_idle(body_t* body)
{
    body_set_velocity(body, (vector_t) {0, body_get_velocity(body).y});
}


// Applies a jumping impulse to the player if the player is in contact with a
// surface from below.
void player_jump(body_t *body)
{
    sprite_t *player = (sprite_t*) (body_get_info(body));
    contact_t contact = sprite_contact(player);

    // Only proceed if touching
    if (!contact.below) {return;}
    Mix_PlayChannel(-1, gJump, 0);
    // Sets jumping based on velocity
    (body_get_velocity(body).x > 0) ?
        sprite_set_state(player, PLAYER_JUMPING_RIGHT) : sprite_set_state(player, PLAYER_JUMPING_LEFT);

    body_translate(body, (vector_t) {0, 2}); // Bump to stop collision
    body_add_impulse(body, JUMP_IMPULSE);
}

void player_downpound(body_t* body)
{
    sprite_t *player = (sprite_t*) (body_get_info(body));
    contact_t contact = sprite_contact(player);
    // You can only ground pound if your in the air

    vector_t v = body_get_velocity(body);
    if (v.x > 0 && !sprite_state_equal(player, PLAYER_GROUNDPOUND_RIGHT)) {sprite_set_state(player, PLAYER_GROUNDPOUND_RIGHT);}
    else if (v.x < 0 && !sprite_state_equal(player, PLAYER_GROUNDPOUND_LEFT)) {sprite_set_state(player, PLAYER_GROUNDPOUND_LEFT);}
    if (sprite_state_equal(player, PLAYER_IDLE_RIGHT)) {sprite_set_state(player, PLAYER_GROUNDPOUND_RIGHT);}
    if (sprite_state_equal(player, PLAYER_IDLE_LEFT)) {sprite_set_state(player, PLAYER_GROUNDPOUND_LEFT);}
    if (contact.below) {return;}
    // Only add down impluse if not touching ground
    body_add_impulse(body, vec_negate(DOWN_IMPULSE));
    sprite_immobilized_tick(player);
}

// Moves player in the specified direction at constant speed.
void player_move(body_t *body, double direction)
{
    sprite_t *sprite = (sprite_t*) (body_get_info(body));
    contact_t contact = sprite_contact(sprite);

    // Immobilizes players due to an enemy collision
    if(sprite_is_immobilized(sprite) != 0) {return;}

    // Bump to stop collision and get out of boundary
    // Only apply this bump if player is moving to the right
    if (contact.left && direction == RIGHT) {body_translate(body, (vector_t) {2, 0});}
    if (contact.right && direction == LEFT) {body_translate(body, (vector_t) {-2, 0});}
    body_set_velocity(body, (vector_t) {direction, body_get_velocity(body).y});

}

// Player releases a fireball
void player_fireball(scene_t *scene, body_t* body)
{
    sprite_t *sprite = (sprite_t*) (body_get_info(body));
    // the +6 is for player 2
    if (sprite_current_frame(sprite).x != FIRE_MODE && sprite_current_frame(sprite).x != FIRE_MODE + 6) {return;}
    Mix_PlayChannel(-1, gFireball, 0);
    vector_t position = body_get_centroid(body);
    vector_t v = body_get_velocity(body);
    if (v.x > 0) {add_fireball(scene, (vector_t) {position.x + 12.5, position.y}, FIREBALL_RIGHT);}
    if (v.x < 0) {add_fireball(scene, (vector_t) {position.x - 12.5, position.y}, FIREBALL_LEFT);}
    if (sprite_state_equal(sprite, PLAYER_IDLE_RIGHT)) {add_fireball(scene, (vector_t) {position.x + 18.5, position.y}, FIREBALL_RIGHT);}
    if (sprite_state_equal(sprite, PLAYER_IDLE_LEFT)) {add_fireball(scene, (vector_t) {position.x - 18.5, position.y}, FIREBALL_LEFT);}
}

// Key handler function. key handler needs to be in the main file it looks like,
// cause I got a linker command error :/
void on_key(char key, key_event_type_t type, double held_time, scene_t *scene)
{
    body_t* player = scene_get_body(scene, PLAYER, 0);

    if (type == KEY_PRESSED) // idea for two people, maybe just add a for loop to go over this twice?
    {
        switch(key)
        {
            case 'w': // jump
                player_jump(player);
                break;
            case 'd': // right
                player_move(player, RIGHT);
                break;
            case 'a': // left
                player_move(player, LEFT);
                break;
            case 's':
                player_downpound(player);
                break;
            case 'f':
                player_fireball(scene, player);
                break;
            case 'q': // launches in game menu
                menu_ingame();
                break;
            case '9':
                // Plays the music
                if (Mix_PlayingMusic() == 0) {Mix_PlayMusic(gMenuMusic, -1);}
                else 
                {
                    // resumes music
                    if (Mix_PausedMusic() == 1) {Mix_ResumeMusic();}
                    // Pauses music
                    else {Mix_PauseMusic();}
                }
                break;
            case '8':
                // stops music
                Mix_HaltMusic();
                break;
        }
    }
    // If a key was released then go back to idle mode
    if (type == KEY_RELEASED) 
    {
        switch(key)
        {
            case 'w': // jump
                player_idle(player);
                break;
            case 'd': // right
                player_idle(player);
                break;
            case 'a': // left
                player_idle(player);
                break;
            case 's':
                player_idle(player);
                break;
        }
    }
}

void multiplayer_on_key(char key, key_event_type_t type, double held_time, scene_t *scene)
{
    body_t* player = scene_get_body(scene, PLAYER, 0);
    body_t* player2 = scene_get_body(scene, PLAYER, 1);


    if (type == KEY_PRESSED) // idea for two people, maybe just add a for loop to go over this twice?
    {
        switch(key)
        {
            case 'w': // jump
                player_jump(player);
                break;
            case 'd': // right
                player_move(player, RIGHT);
                break;
            case 'a': // left
                player_move(player, LEFT);
                break;
            case 's':
                player_downpound(player);
                break;
            case 'f':
                player_fireball(scene, player);
                break;
            case '9':
                // Plays the music
                if (Mix_PlayingMusic() == 0) {Mix_PlayMusic(gMenuMusic, -1);}
                else 
                {
                    // resumes music
                    if (Mix_PausedMusic() == 1) {Mix_ResumeMusic();}
                    // Pauses music
                    else {Mix_PauseMusic();}
                }
                break;
            case '8':
                // stops music
                Mix_HaltMusic();
                break;
        }
        switch(key)
        {
            case UP_ARROW: // jump
                player_jump(player2);
                break;
            case RIGHT_ARROW: // right
                player_move(player2, RIGHT);
                break;
            case LEFT_ARROW: // left
                player_move(player2, LEFT);
                break;
            case DOWN_ARROW:
                player_downpound(player2);
                break;
            case '/':
                player_fireball(scene, player2);
                break;
            case 'q': // launches in game menu
                menu_ingame();
                break;
        }
    }
    // If a key was released then go back to idle mode
    if (type == KEY_RELEASED)
    {
        switch(key)
        {
            case 'w': // jump
                player_idle(player);
                break;
            case 'd': // right
                player_idle(player);
                break;
            case 'a': // left
                player_idle(player);
                break;
            case 's':
                player_idle(player);
                break;
        }
        switch(key)
        {
            case UP_ARROW: // jump
                player_idle(player2);
                break;
            case RIGHT_ARROW: // right
                player_idle(player2);
                break;
            case LEFT_ARROW: // left
                player_idle(player2);
                break;
            case DOWN_ARROW:
                player_idle(player2);
                break;
        }
    }
}
