#include "levels.h"
#include "scene.h"
#include "sdl_wrapper.h"
#include "leaderboard.h"
#include "gameplay.h"
#include "menu.h"
#include "test_util.h"
#include <SDL2/SDL_mixer.h>

const vector_t MIN = {0, 0};
const vector_t MAX = {1000, 500};
// cut scenes are only played during level 1
const double LEVEL_1 = 1;

// If player is within 50 pixels of end they win
const double END_DISTANCE = 100;

// If player wants to continue to next level
bool NEXT_LEVEL = false;
extern const int MAIN_MENU;
extern const int QUIT_VALUE;
int player_choice = -1;
const double LEADERBOARD = 0;
const double LEVELS = 1;

// Translates every object in scene
void move_camera(scene_t *scene, double x, double y)
{
    // Starts at 1 because you don't move background
    for (size_t role = 0; role < NUM_ROLES; role ++)
    {
        list_t *role_list = scene_get_list(scene, role);
        for (int i = 0; i < list_size(role_list); i++)
        {
            body_t *body = (body_t*) list_get(role_list, i);
            body_translate(body, (vector_t) {x, y});
        }
    }
}
// Tracks player movement by translating entire scene
void track_player(scene_t *scene)
{
    body_t *player = scene_get_body(scene, PLAYER, 0);
    vector_t playerPos = body_get_centroid(player);
    list_t *platform_list = scene_get_list(scene, PLATFORM);
    double leftBound = body_get_centroid(list_get(platform_list, 2)).x;
    double rightBound = body_get_centroid(list_get(platform_list, 3)).x;
    // Only translates scene when player is not b/w 250-750(x-axis) of shown screen
    if (leftBound < 0 && playerPos.x < 250) {move_camera(scene, 250 - playerPos.x, 0);}
    if (rightBound > 1000 && playerPos.x > 750) {move_camera(scene, 750 - playerPos.x, 0);}
    // Handles edge case so that camera stops at border and doesn't go off screen
    if (leftBound > 0) {move_camera(scene, 0 - leftBound, 0);}
    if (rightBound < 1000) {move_camera(scene, 1000 - rightBound, 0);}
}

// updates entry fields
void game_entry_update(sprite_t* player, entry_t* entry)
{
    if (entry == NULL) {return;}
    entry_set_kills(entry, sprite_kills(player));
    entry_set_tokens(entry, sprite_tokens(player));
}

// Updates some metrics and returns t/f if player reached end
// I couldn't really think of a nice way to do this
bool game_is_over(scene_t* scene, bool* victory, entry_t* entry1, entry_t* entry2, bool multiplayer)
{
    if(scene == NULL) {return false;}
    body_t* body_player = scene_get_body(scene, PLAYER, 0);
    sprite_t* player = (sprite_t*) body_get_info(body_player);
    if (multiplayer)
    {
        body_t* body_player2 = scene_get_body(scene, PLAYER, 1);
        sprite_t* player2 = (sprite_t*) body_get_info(body_player2);
        game_entry_update(player, entry1);
        game_entry_update(player2, entry2);
        if (body_get_centroid(body_player).x + END_DISTANCE >= body_get_centroid(scene_get_body(scene, PLATFORM, 3)).x || body_get_centroid(body_player2).x + END_DISTANCE >= body_get_centroid(scene_get_body(scene, PLATFORM, 3)).x)
        {
            // Updates entry
            *victory = true;
            return true;
        }
        if (sprite_health(player) == 0 || sprite_health(player2) == 0) {return true;}

        return false;
        // We would need to switch tracking in this case
    }
    else
    {
        game_entry_update(player, entry1);
        // Player is w/in 50 pixels of end level, so they won
        if (body_get_centroid(body_player).x + END_DISTANCE >= MAX.x)
        {
            *victory = true;
            return true;
        }
        if (sprite_health(player) == 0) { return true;}
        return false;
    }
}
// updates player from scen2 based on player from scene 1, this keeps transfer of stats to sublevel and back
void update_players(scene_t* scene, scene_t* scene2, bool multiplayer)
{
    sprite_t* player_copying = (sprite_t*) body_get_info(scene_get_body(scene, PLAYER, 0));
    sprite_t* player_updating = (sprite_t*) body_get_info(scene_get_body(scene2, PLAYER, 0));
    sprite_update(player_updating, sprite_kills(player_copying), sprite_tokens(player_copying), sprite_health(player_copying));
    if(multiplayer)
    {
        sprite_t* player_copying2 = (sprite_t*) body_get_info(scene_get_body(scene, PLAYER, 1));
        sprite_t* player_updating2 = (sprite_t*) body_get_info(scene_get_body(scene2, PLAYER, 1));
        sprite_update(player_updating2, sprite_kills(player_copying2), sprite_tokens(player_copying2), sprite_health(player_copying2));
    }
}

// Runs a sublevel within game
void run_sublevel(bool multiplayer, entry_t* entry_player1, entry_t* entry_player2, scene_t* scene, bool* sublevel)
{
    scene_t* scene2 = sublevel_load(multiplayer);
    if (scene2 == NULL) {return;} // stops function
    bool victory = false;
    *sublevel = true;
    update_players(scene, scene2, multiplayer); // updates players in scene 2
    sdl_clear();
    while(player_choice >= LEVELS && !game_is_over(scene2, &victory, entry_player1, entry_player2, multiplayer) && menu_choice() != QUIT_VALUE)
    {
        assert(scene2 != NULL);
        player_choice = menu_choice(); // Can be altered ingame to quit level
        if (multiplayer) {sdl_on_key((key_handler_t) multiplayer_on_key);}
        else {sdl_on_key((key_handler_t) on_key);}
        double dt = time_since_last_tick();
        scene_tick(scene2, dt);
        sdl_render_scene(scene2);
        text_level_info(scene2, multiplayer);
        sdl_render();
        track_player(scene2); // handles in game camera
        if(sdl_is_done(scene2)) {player_choice = menu_quit(); break;} // need to break or next line changes player-choice
    }
    sdl_clear();
    // Only return if scene is non-Null
    update_players(scene2, scene, multiplayer); // updates players in scene 1
    body_t* player = scene_get_body(scene, PLAYER, 0);
    body_set_centroid(player, (vector_t) {5980, 100}); // this is for sub level 2
    // Due to some diffculties and because we ran out of time we had to make this concession....
    if (multiplayer)
    {
        body_t* player2 = scene_get_body(scene, PLAYER, 1);
        vector_t v1 = body_get_centroid(player);
        vector_t v2 = body_get_centroid(player2);
        if (!vec_isclose(v1, v2)) 
        {
            (v1.x > v2.x) ? 
                body_set_centroid(player2, v1) : body_set_centroid(player, v2); 
        }
    }
    free(scene2);
}


// Keys for when making a yes or no question
void binary_on_key(char key, key_event_type_t type, double held_time, scene_t *scene)
{
    if (type == KEY_PRESSED) // idea for two people, maybe just add a for loop to go over this twice?
    {
        switch(key)
        {   // check if this work not sure
            case RETURN_KEY:
                NEXT_LEVEL = true;
                break;
            case ESCAPE_KEY:
                player_choice = MAIN_MENU;
                break;
        }
    }
}

// Game loop ------------------------------------------------------------------
int main()
{
    vector_t min = {0, 0};
    vector_t max = {1000, 500};
    sdl_init(min, max);

    bool first_round = true;
    bool leaderboard = false; // Will get set to true if player goes to leaderboard
    bool victory = false;
    bool name_set = true;
    char* name1;
    char *name2;
    int multiplayer = menu_multiplayer();
    while(player_choice != QUIT_VALUE && multiplayer != QUIT_VALUE && menu_choice() != QUIT_VALUE)
    {
        sdl_clear();
        leaderboard = false; // resets to false each loop
        // Skips loading menu if player wants to go to next level and won last level
        // player_choices for menu are: leaderboard, level 1/2/3, and quit
        if (player_choice == MAIN_MENU || !victory) {player_choice = menu_load(first_round);}
        NEXT_LEVEL = false; victory = false;// resets to false after check
        if (player_choice == QUIT_VALUE) {break;} // breaks loop early
         // 2-player mode on or off --- not yet implemented

        if (name_set && player_choice != LEADERBOARD) // Assures that player is only asked for name once
        {
            name1 = text_username_input(1); // 1 stands for player 1
            if (multiplayer) {name2 = text_username_input(2);} // 2 stands for player 2
            name_set = false;
        }
        sdl_clear(); // Clear text after

        if (player_choice == LEVEL_1) {opening_story();}
        if (first_round) {text_directions();}
        if (player_choice >= LEVELS) {first_round = false;} // Player has played a level
        // Select level or returns a NULL scene
        scene_t* scene = level_select(player_choice, multiplayer);
        entry_t* entry_player1 = entry_init(name1);
        entry_t* entry_player2 = entry_init(name2);

        time_since_last_tick(); // Need to call this before so dt gets reset
        // If not the first tick will be 1 to 2 seconds vs 0.2
        // Main level loop
        bool sublevel_ran = false;
        while(player_choice >= LEVELS && !game_is_over(scene, &victory, entry_player1, entry_player2, multiplayer) && menu_choice() != QUIT_VALUE)
        {
            assert(scene != NULL);
            player_choice = menu_choice(); // Can be altered ingame to quit level
            // Always keep resetting keys, because keys switch when payer uses in game menu
            if (multiplayer) {sdl_on_key((key_handler_t) multiplayer_on_key);}
            else {sdl_on_key((key_handler_t) on_key);}

            double dt = time_since_last_tick();
            scene_tick(scene, dt);
            sdl_render_scene(scene);
            text_level_info(scene, multiplayer);
            sdl_render();
            if(!sublevel_ran) {run_sublevel(multiplayer, entry_player1, entry_player2, scene, &sublevel_ran);}
            track_player(scene); // handles in game camera
            if(sdl_is_done(scene)) {player_choice = menu_quit(); break;} // need to break or next line changes player-choice
        }

        if(player_choice >= LEVELS) {scene_free(scene);} // only free when level selected
        // Loads leaderboard if that option selected in menu

        if (victory) // Update leaderboard if they won
        {
            Mix_PlayChannel(-1, gVictory, 0);
            leaderboard_update(entry_player1);
            if (multiplayer) {leaderboard_update(entry_player2);}
        }
        sdl_on_key((key_handler_t) binary_on_key); // Sets keys for next prompt
        if (player_choice == 3 && victory)
        {
            while(player_choice != MAIN_MENU)
            {
                sdl_clear();
                if(sdl_is_done(NULL)) {player_choice = menu_quit();}
                text_final_level(); // they beat the game!
                sdl_render();
            }
            player_choice = MAIN_MENU;
            NEXT_LEVEL = false;
        }

        
        while(player_choice != MAIN_MENU && !NEXT_LEVEL && player_choice != QUIT_VALUE && menu_choice() != QUIT_VALUE)
        {
            // Display win or loss 
            if (victory) {you_won();}
            else if (player_choice != LEADERBOARD) {Mix_PlayChannel(-1, gDeath, 0); you_lost();}
            if (player_choice == LEADERBOARD) {sdl_clear(); leaderboard = leaderboard_show();}
            sdl_render(); 
            if(sdl_is_done(NULL)) {player_choice = menu_quit();}
        }

        if (NEXT_LEVEL)
        {   // Player advances to next level until hitting level 3 (max level)
            if (player_choice < 3) {player_choice ++;}
        }
        if(sdl_is_done(NULL)) {player_choice = menu_quit(); break;}
    }
    if (multiplayer == QUIT_VALUE) {sdl_cleanup();}
}
