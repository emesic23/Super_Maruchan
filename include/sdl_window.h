#ifndef __SDL_WINDOW_H__
#define __SDL_WINDOW_H__

#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>


// Should I move this an write accessors?
typedef struct window_info
{
    SDL_Window* window;
    vector_t center;
    vector_t window_center;

    // The coordinate difference from the center to the top right corner.
    vector_t max_diff;
    double scale;

} window_info_t;


// Initializes a window_info_t struct. Returns success or failure.
int window_info_init(vector_t min, vector_t max, window_info_t* window_info);

void window_info_camera(vector_t min, vector_t max, window_info_t* window_info);

void window_info_free(window_info_t* window_info);

// Computes the scaling factor between scene coordinates and pixel coordinates.
// The scene is scaled by the same factor in the x and y dimensions,
// chosen to maximize the size of the scene while keeping it in the window.
double get_scene_scale(window_info_t window_info);


// Maps a scene coordinate to a window coordinate.
vector_t get_window_position(vector_t scene_pos, window_info_t window_info);


// Computes the center of the window in pixel coordinates.
vector_t get_window_center(SDL_Window* window);

#endif
