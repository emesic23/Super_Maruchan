#include <stdlib.h>
#include <stdio.h>
#include "controls.h"
#include "menu.h"
#include "sdl_wrapper.h"
#include "sdl_draw.h"
#include "list.h"
#include "scene.h"
#include "body.h"
#include "vector.h"
#include "color.h"
#include "string.h"
#include "initialize.h"
#include <SDL2/SDL_mixer.h>


extern Mix_Music* gMenuMusic;
// window sizes
extern const vector_t MAX;
extern const vector_t MIN;

// list init size
extern const double INITIAL_SIZE;

// At startup Quit is always false
const int QUIT_VALUE = -2;

// Colors
extern const rgb_color_t ORANGE;
extern const rgb_color_t BLUE;
const rgb_color_t BLACK = (rgb_color_t) {0, 0, 0};
const rgb_color_t YELLOW = (rgb_color_t) {1, 1, 0};

// render or not
int restart = -1;
const int NO_KEYPRESS = -1;
double choice = -1;
double ingame_choice = -1;
const int MAIN_MENU = -3;
bool SKIP_CUTSCENES = false;

// Vector positions that each line is rendered at
const vector_t LINE1 = (vector_t) {500, 450};
const vector_t LINE2 = (vector_t) {500, 430};
const double LINE_INC = -20;

bool name_done = false;

// Keys handling for when using the menu.
void menu_on_key(char key, key_event_type_t type, double held_time, scene_t *scene)
{
    if (type == KEY_PRESSED)
    {
        switch(key)
        {
            case '0': // Opens leaderboard...
                choice = 0; 
                break; 
            case '1': // Level 1
                choice = 1; 
                break; 
            case '2': // Level 2
                choice = 2; 
                break; 
            case '3': // Level 3
                choice = 3; 
                break; 
            case '4': // Quits game...
               choice = menu_quit();
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
}

void menu_ingame_key(char key, key_event_type_t type, double held_time, scene_t *scene)
{
    if (type == KEY_PRESSED)
    {
        switch(key)
        {
            case '0': // proceed with level
                ingame_choice = 0; 
                break; 
            case '1': // return to main menu
                ingame_choice = MAIN_MENU; 
                break; // Level 1
            case '2': // Quits game...
                ingame_choice = menu_quit();
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
}

// Keys so user can skip cutscene
void cutscenes_on_key(char key, key_event_type_t type, double held_time, scene_t *scene)
{
    if (type == KEY_PRESSED)
    {
        switch(key)
        {
            case ESCAPE_KEY: // proceed with level
                SKIP_CUTSCENES = true; 
                break; 
            case RETURN_KEY:
                SKIP_CUTSCENES = true;
                break;
        }
    }
}

// Quits everywhere
double menu_quit()
{
    // sdl_cleanup();
    sdl_cleanup();
    return -2;
}

int menu_load(bool first_visit)
{
    // Starts playing Menu Masic
    choice = NO_KEYPRESS;
    // Loads menu text and then renders it

    sdl_on_key((key_handler_t) menu_on_key);
    // Renders menu text until a keypress is recieved
    
    // While loop waiting for key input
    while(choice == NO_KEYPRESS) 
    {
        // Two options for top text
        if (first_visit) {text_opening1();}
        // Message for second time you return to menu
        else {text_opening2();}
        text_menu();
        sdl_render(); 
        if(sdl_is_done(NULL)) {return menu_quit();}
    }
    if (!first_visit) {Mix_HaltMusic();} // Stops menu music
    return choice;
}

// Asks if playing single or multiplayer
bool menu_multiplayer()
{
    Mix_PlayMusic(gMenuMusic, -1);
    ingame_choice = NO_KEYPRESS; // resets
    sdl_on_key((key_handler_t) menu_ingame_key);
    // Renders menu text until a keypress is recieved
    // While loop waiting for key input
    while(ingame_choice == NO_KEYPRESS) {
        text_multiplayer_menu();
        sdl_render(); 
        if(sdl_is_done(NULL)) {choice = menu_quit(); return QUIT_VALUE;}
        }
    
    time_since_last_tick();
    if(ingame_choice == MAIN_MENU) {return true;}
    if(ingame_choice == false) {return false;}
    if(ingame_choice == QUIT_VALUE) {choice = QUIT_VALUE; return QUIT_VALUE;}
}

// In game menu
void menu_ingame()
{
    sdl_clear();
    ingame_choice = NO_KEYPRESS; // resets
    sdl_on_key((key_handler_t) menu_ingame_key);
    // Renders menu text until a keypress is recieved
    // While loop waiting for key input
    while(ingame_choice == NO_KEYPRESS) 
    {
        text_ingame_menu();
        sdl_render();
        if(sdl_is_done(NULL)) {choice = menu_quit();}
    }
    if(ingame_choice == MAIN_MENU) {choice = MAIN_MENU;}
    if(ingame_choice == QUIT_VALUE) {choice = QUIT_VALUE;}
    time_since_last_tick();
    sdl_clear();
}
// shows directions
void text_directions()
{
    SKIP_CUTSCENES = false;
    sdl_clear();
    ingame_choice = NO_KEYPRESS; // resets
    sdl_on_key((key_handler_t) menu_ingame_key);
    // Renders menu text until a keypress is recieved
    text_ingame_menu();
    sdl_render(); 
    double dt = 0;
    scene_t* scene = scene_init();
    add_background(scene, (vector_t) {1000, 500}, CUTSCENE8);
    sdl_on_key((key_handler_t) cutscenes_on_key);
    // While loop that can either be skipped or cycles through the opening scenes
    while (!SKIP_CUTSCENES)
    {   
        sdl_render_scene(scene);
        sdl_render();
        {if(sdl_is_done(NULL)) {choice = menu_quit();}}
    }
    scene_free(scene);
    time_since_last_tick();
    sdl_clear();
}


// Shows opening story
void opening_story()
{
    SKIP_CUTSCENES = false;
    sdl_clear();
    ingame_choice = NO_KEYPRESS; // resets
    sdl_on_key((key_handler_t) menu_ingame_key);
    // Renders menu text until a keypress is recieved
    text_ingame_menu();
    sdl_render(); 
    double dt = 0;
    scene_t* scene = scene_init();
    add_background(scene, (vector_t) {1000, 500}, CUTSCENE1);
    sdl_on_key((key_handler_t) cutscenes_on_key);
    // While loop that can either be skipped or cycles through the opening scenes
    while (!SKIP_CUTSCENES && dt < 140)
    {   
        if (dt >= 20)
        {
            body_remove(scene_get_body(scene, BACKGROUND, 0));            
            add_background(scene, (vector_t) {1000, 500}, CUTSCENE2);
        }
        if (dt >= 40)
        {
            body_remove(scene_get_body(scene, BACKGROUND, 0));
            add_background(scene, (vector_t) {1000, 500}, CUTSCENE3);
        }
        if (dt >= 60)
        {
            body_remove(scene_get_body(scene, BACKGROUND, 0));
            add_background(scene, (vector_t) {1000, 500}, CUTSCENE4);
        }
        if (dt >= 80)
        {
            body_remove(scene_get_body(scene, BACKGROUND, 0));
            add_background(scene, (vector_t) {1000, 500}, CUTSCENE5);
        }
        if (dt >= 100)
        {
            body_remove(scene_get_body(scene, BACKGROUND, 0));
            add_background(scene, (vector_t) {1000, 500}, CUTSCENE6);
        }
        if (dt >= 120)
        {
            body_remove(scene_get_body(scene, BACKGROUND, 0));
            add_background(scene, (vector_t) {1000, 500}, CUTSCENE7);
        }
        dt += time_since_last_tick();
        sdl_render_scene(scene);
        sdl_render();
        {if(sdl_is_done(NULL)) {choice = menu_quit();}}
    }
    scene_free(scene);
    time_since_last_tick();
    sdl_clear();
}

// constantly returning value of choice
int menu_choice()
{
    return choice;
}

// Text stuff --------------------------------

// Text opening for first time entering game
void text_opening1()
{
    sdl_write_title("Welcome to SOUPER MARUCHAN. ", ORANGE, LINE1);
    sdl_write_regular("We are so excited for you to join us!", ORANGE, LINE2);
}

// Text opening for after completing level or dying
void text_opening2()
{
    sdl_write_title("SOUPER MARUCHAN", ORANGE, LINE1);
    sdl_write_regular("Hi again! Hope that level was fun!", ORANGE, LINE2);
}

// Text for main menu
void text_menu()
{
    sdl_write_regular("Each option corresponds to a number key.", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 2});
    sdl_write_regular("You have the following five choices:", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 3});
    sdl_write_regular("    Option 0: Look at leaderboard", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 4});
    sdl_write_regular("    Option 1: enter level1", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 5});
    sdl_write_regular("    Option 2: enter level2", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 6});
    sdl_write_regular("    Option 3: enter level3", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 7});
    sdl_write_regular("    Option 4: Quit game", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 8});
}

// Text for choosing singleplayer or multiplayer mode
void text_multiplayer_menu()
{
    sdl_write_title("SOUPER MARUCHAN", ORANGE, LINE1);
    sdl_write_regular("Choose play mode:", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 3});
    sdl_write_regular("    Option 0: SINGLEPLAYER", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 4});
    sdl_write_regular("    Option 1: MULTIPLAYER", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 5});
    sdl_write_regular("    Option 2: QUIT", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 6});
    sdl_write_regular(" In MULTIPLAYER, if one person dies the game ends for both of you!", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 7});
    sdl_write_regular(" So be careful!!", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 8});  
}

// Text for ingame meny
void text_ingame_menu()
{
    sdl_write_regular("Each option corresponds to a number key.", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 2});
    sdl_write_regular("You have the following three choices:", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 3});
    sdl_write_regular("    Option 0: Continue with level", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 4});
    sdl_write_regular("    Option 1: Return to Main Menu", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 5});
    sdl_write_regular("    Option 2: Quit game", ORANGE, (vector_t) {LINE2.x, LINE2.y + LINE_INC * 6}); 
}
 
// Text opening for after completing level or dying
void you_won()
{
    sdl_write_regular("Congrats on completeing the level!", ORANGE, LINE1);
    sdl_write_regular("Would you like to continue to the next level?", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 2});
    sdl_write_regular("[enter = yes, esc = no]", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 3});
}
// Text for when player dies dying
void you_lost()
{
    sdl_write_regular("Oh no you died! :(", ORANGE, LINE1);
    sdl_write_regular("But don't worry the adventure never ends!", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 2});
    sdl_write_regular("Press esc to continue", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 3});
}

void text_final_level()
{
    sdl_write_title("GAMEOVER", ORANGE, LINE1);
    sdl_write_title("YOU BEAT THE GAME!!!!!", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 2});
    sdl_write_regular("You succesfully got the Golden Ramen back!!!!!!", BLACK, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 3});
    sdl_write_regular("We hope you enjoyed the game as much as we did", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 4});
    sdl_write_regular("Check out the leaderboard to see how you did", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 5});
    sdl_write_regular("Ofc this couldn't have been possible without our amazing team.", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 6});
    sdl_write_title("CREDITS:", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 7.5});
    sdl_write_title("Thanks to our team members: Esmir, Jon, Julen, Pippa", random_color(), (vector_t) {LINE1.x, LINE1.y + LINE_INC * 9});
    sdl_write_title("press esc to leave", ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 10});
}

// Prints text during level of player at top of screen
void text_level_info(scene_t* scene, bool multiplayer)
{
    if(scene == NULL) {return;}
    body_t* body_player = scene_get_body(scene, PLAYER, 0);
    sprite_t* player = (sprite_t*) body_get_info(body_player);
    char* str[25];
    // IF 2 player than top line changes to display info of both players
    if (multiplayer) 
    {
        body_t* body_player2 = scene_get_body(scene, PLAYER, 1);
        sprite_t* player2 = (sprite_t*) body_get_info(body_player2);
        sprintf(&str, "Level tracking: Health: %zu, Player2: %zu         Tokens: %zu, Player2: %zu         Kills: %zu, Player2: %zu", 
            sprite_health(player), sprite_health(player2), sprite_tokens(player), 
            sprite_tokens(player2), sprite_kills(player), sprite_kills(player2));
    } else 
    {
        sprintf(&str, "Level tracking: Health: %zu         Tokens: %zu         Kills: %zu", 
            sprite_health(player), sprite_tokens(player), sprite_kills(player));
    }
    sdl_write_regular(str, ORANGE, LINE1);
}

// Generate a random color.
rgb_color_t random_color()
{
    float r = (float) (rand() % 10) / 10;
    float g = (float) (rand() % 10) / 10;
    float b = (float) (rand() % 10) / 10;
    return (rgb_color_t) {r, g, b};
}

// This next functions all deal with user input for name -------------------------------------------------------

// This handles player typing in their username
char* text_username_input(double player_number)
{
    sdl_on_key((key_handler_t) name_on_key);
    char* name = malloc(sizeof(char) * 30);
    name[0] = '\0';
    char buf[40];
    
    // Once player hits enter, the while loop is exited and name is returned
    while(!name_done && choice != QUIT_VALUE)
    {
        sprintf(buf, "Player %zu Name: %s", (size_t) player_number, name);
        sdl_write_regular("Please type in your username:", ORANGE, LINE1);
        sdl_write_regular("MAX 20 characters and you only get ONE chance so make it count!", ORANGE, LINE2);
        sdl_write_regular(buf, ORANGE, (vector_t) {LINE1.x, LINE1.y + LINE_INC * 3});
        
        sdl_render();
        if(sdl_is_done(name)) {choice = menu_quit();}

        if(strlen(name) >= 21) {break;}
    }
    name_done = false; // reset to false for player2
    Mix_HaltMusic();
    return name;
}

// Key handler for name input
void name_on_key(char key, key_event_type_t type, double held_time, char* name)
{
    if (type == KEY_PRESSED)
    {
        if (key == RETURN_KEY) {name_done = true;}
        else 
        {
            char str[2] = {key, '\0'};
            // Might have to do some passing by reference or something here.
            name = strcat(name, str);
        }
        if (key == ESCAPE_KEY) {choice = QUIT_VALUE;}
    }
}
