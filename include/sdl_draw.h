#ifndef __SDL_DRAW_H__
#define __SDL_DRAW_H__

#include "constants.h"
#include "sdl_window.h"
#include "vector.h"
#include "color.h"
#include "shape.h"
#include "sprite.h"
#include "body.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <stdlib.h>

// Takes a texture, the role associated with the texture, and the list of 
// frames as input. This basically just initializes the size of a frame for 
// each sprite sheet.
void sdl_frames_init(SDL_Texture* texture, role_t role, list_t* frames);

// Loads the fonts from the resources/fonts directory.
// int sdl_fonts_init(TTF_Font* title_font, TTF_Font* regular_font);
int sdl_textures_init(list_t* textures, list_t* frames, SDL_Renderer* renderer);


// Adds the inputted text to the backbuffer.
void sdl_write(TTF_Font* font, 
        char* string, 
        rgb_color_t color, 
        vector_t position, 
        SDL_Renderer* renderer, 
        window_info_t window_info);


// Draws a sprite on the window.
void sdl_draw_sprite(body_t* body, list_t* textures, list_t* frames, SDL_Renderer* renderer, window_info_t window_info);


// Draws a shape_t object on the window.
void sdl_draw_shape(shape_t* shape, rgb_color_t color, SDL_Renderer* renderer, window_info_t window_info);


// Draws a CIRCLE shape_t object on the window.
void sdl_draw_circle(shape_t* shape, rgb_color_t color, SDL_Renderer* renderer, window_info_t window_info);


// Draws a POLYGON shape_t object on the window.
void sdl_draw_polygon(shape_t* shape, rgb_color_t color, SDL_Renderer* renderer, window_info_t window_info);

#endif
