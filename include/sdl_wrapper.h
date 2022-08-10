#ifndef __SDL_WRAPPER_H__
#define __SDL_WRAPPER_H__

#include "shape.h"
#include "scene.h"
#include "sdl_window.h"
#include "sdl_draw.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


// Values passed to a key handler when the given arrow key is pressed
#define LEFT_ARROW 1
#define UP_ARROW 2
#define RIGHT_ARROW 3
#define DOWN_ARROW 4
#define RETURN_KEY -4
#define ESCAPE_KEY -5

// The possible types of key events. Enum types in C are much more primitive than in Java; 
// this is equivalent to: 
// typedef unsigned int KeyEventType; 
// #define KEY_PRESSED 0
// #define KEY_RELEASED 1
typedef enum 
{
    KEY_PRESSED,
    KEY_RELEASED
} key_event_type_t;

extern Mix_Music *gMenuMusic;
extern Mix_Chunk *gToken;
extern Mix_Chunk *gJump;
extern Mix_Chunk *gPowerUp;
extern Mix_Chunk *gDeath;
extern Mix_Chunk *gVictory;
extern Mix_Chunk *gWarpPipe;
extern Mix_Chunk *gHealth;
extern Mix_Chunk *gFireball;

// A keypress handler. When a key is pressed or released, the handler is passed its char value.
// Most keys are passed as their char value, e.g. 'a', '1', or '\r'.
// Arrow keys have the special values listed above.
typedef void (*key_handler_t)(char key, key_event_type_t type, double held_time, void* passer);


// loads audio
void load_audio();

// Initializes the SDL window and renderer. Must be called once before any of the other SDL functions.
// min: the x and y coordinates of the bottom left of the scene
// max: the x and y coordinates of the top right of the scene
void sdl_init(vector_t min, vector_t max);


// Processes all SDL events and returns whether the window has been closed. This function must be 
// called in order to handle keypresses.
bool sdl_is_done(void* passer);

// Being used for testing currently for jon
bool sdl_is_done_testing(void* passer);


// Clears the screen. Should be called before drawing polygons in each frame.
void sdl_clear(void);


// Displays the rendered frame on the SDL window. Must be called after drawing the polygons in order to show them.
void sdl_show(void);


// Draws all bodies in a scene. This internally calls sdl_clear(), sdl_draw_polygon(), and sdl_show().
void sdl_render_scene(scene_t *scene);

// Clears the SDL window, and renders menu
void sdl_render();

// Registers a function to be called every time a key is pressed. Overwrites any existing handler.
void sdl_on_key(key_handler_t handler);


// Gets the amount of time that has passed since the last time this function was called, in seconds.
double time_since_last_tick(void);



// Text rendering ----------------------------------------

// Structure for rendering text.
typedef struct text
{
    vector_t position; // Where text will be rendered 
    char* string; //Stores the message
    rgb_color_t color; // Color of text
} text_t;


void sdl_write_title(char* string, rgb_color_t color, vector_t position);
void sdl_write_regular(char* string, rgb_color_t color, vector_t position);

// Cleans up and frees everything
void sdl_cleanup();

#endif // #ifndef __SDL_WRAPPER_H__
