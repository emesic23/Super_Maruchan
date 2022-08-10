#ifndef __MENU_H__
#define __MENU_H__

#include "color.h"
#include "scene.h"
#include "sdl_wrapper.h"

// Keys handling for when using the menu.
void menu_on_key(char key, key_event_type_t type, double held_time, scene_t *scene);
// Keys for handling ingame menu and also used for choosing play mode
void menu_ingame_key(char key, key_event_type_t type, double held_time, scene_t *scene);
// Keys so user can skip a cutscene
void cutscenes_on_key(char key, key_event_type_t type, double held_time, scene_t *scene);

// Loads the menu for the player
// Has different messages if its the player first session in game or not
// Will return int that signifies players choice
int menu_load(bool first_visit);

// Asks if playing single or multiplayer
bool menu_multiplayer();

// shows directions
void text_directions();

// In game menu: continue with level, quit level, or quit game
void menu_ingame();

// Constantly returns value of choice
int menu_choice();

// Quits everywhere
double menu_quit();

// text lists -------------------------------------------------------------
// Shows opening story
void opening_story(); // Runs opening story scenes
void text_opening1(); // Text opening for first time entering game
void text_opening2(); // Text opening for after player death
void text_menu(); // Text for main menu
void text_multiplayer_menu(); // Text for choosing singleplayer or multiplayer mode
void text_ingame_menu(); // Text for ingame meny
void you_won(); // Text for level complete
void you_lost(); // Text for when player dies dying
void text_final_level();
void text_level_info(scene_t* scene, bool multiplayer); // prints text during level

// Key handler for name input
void name_on_key(char key, key_event_type_t type, double held_time, char* name);

// Accepts user input for player name
// Identify if player 1 or 2
char* text_username_input(double player_number);
// Generate a random color.
rgb_color_t random_color();
#endif
