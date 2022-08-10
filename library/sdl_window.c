#include "sdl_window.h"

const char* WINDOW_TITLE = "SOUPer Maruchan Bros";
const int WINDOW_WIDTH = 1000;
const int WINDOW_HEIGHT = 500;


// Stores window_info in the window_info_t struct. Returns 0 if initialization
// was successful and 1 otherwise.
int window_info_init(vector_t min, vector_t max, window_info_t* window_info)
{
    // Make sure to assign memory to window_info.
    if (window_info == NULL) {return 1;}

    // Initialize the SDL window.
    SDL_Window* window = SDL_CreateWindow(
            WINDOW_TITLE,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_RESIZABLE);
    // If the initialization was unsuccessful, return 1.
    if (window == NULL) {return 1;}
    else {window_info -> window = window;}

    window_info -> window_center = get_window_center(window_info -> window);
    window_info -> center = vec_multiply(0.5, vec_add(min, max));
    window_info -> max_diff = vec_subtract(max, window_info -> center);
    window_info -> scale = get_scene_scale(*window_info);

    return 0;
}

void window_info_camera(vector_t min, vector_t max, window_info_t* window_info)
{
    window_info -> window_center = get_window_center(window_info -> window);
    window_info -> center = vec_multiply(0.5, vec_add(min, max));
    window_info -> max_diff = vec_subtract(max, window_info -> center);
    window_info -> scale = get_scene_scale(*window_info);
}

// Frees a window_info_t struct.
void window_info_free(window_info_t* window_info)
{
    SDL_DestroyWindow(window_info -> window);
    free(window_info);
}


// Computes the center of the window in pixel coordinates.
vector_t get_window_center(SDL_Window* window)
{
    int *width = malloc(sizeof(int));
    int *height = malloc(sizeof(int));
    assert(width != NULL);
    assert(height != NULL);

    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {*width, *height};

    free(width);
    free(height);

    return vec_multiply(0.5, dimensions);
}


// Maps a scene coordinate to a window coordinate.
vector_t get_window_position(vector_t scene_pos, window_info_t window_info)
{
    // Scale scene coordinates by the scaling factor
    // and map the center of the scene to the center of the window
    vector_t scene_center_offset = vec_subtract(scene_pos, window_info.center);
    vector_t pixel_center_offset = vec_multiply(window_info.scale, scene_center_offset);

    // Flip y axis since positive y is down on the screen
    vector_t pixel = {round(window_info.window_center.x + pixel_center_offset.x),
        round(window_info.window_center.y - pixel_center_offset.y)};

    return pixel;
}


// NOTE: [What exactly is this doing?]
//
// Computes the scaling factor between scene coordinates and pixel coordinates.
// The scene is scaled by the same factor in the x and y dimensions,
// chosen to maximize the size of the scene while keeping it in the window.
double get_scene_scale(window_info_t window_info)
{
    vector_t max_diff = window_info.max_diff;
    vector_t window_center = window_info.window_center;

    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x;
    double y_scale = window_center.y / max_diff.y;

    // NOTE: This reads like, "If x_scale is less than y_scale, return x_scale
    // else return y_scale."
    return (x_scale < y_scale) ? x_scale : y_scale;
}


