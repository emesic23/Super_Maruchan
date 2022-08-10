#include "sdl_wrapper.h"
#include "sdl_window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

const double MS_PER_S = 1e3;

// Stores information about the SDL window.
window_info_t* window_info;

// The renderer used to draw the scene.
SDL_Renderer* renderer;


// Loaded media -----------------------------------------------------

// Grpahics --------------------------

// The fonts to be used in the game, loaded during initialization.
TTF_Font* TITLE_FONT;
TTF_Font* REGULAR_FONT;
// A list of textures to be used to animate sprites. Each sprite is associated 
// with a texture by role_t.
list_t* textures;
// Stores a list of SDL_Rect, which can be used to "grab" a frame from the 
// sprite sheet. Each rectangle corresponds to a texture in the textures list 
// by index.
list_t* frames;

// Audio ----------------------------

//The music that will be played
Mix_Music *gMenuMusic;
Mix_Music *gLevelMusic;
Mix_Chunk *gToken;
Mix_Chunk *gJump;
Mix_Chunk *gPowerUp;
Mix_Chunk *gDeath;
Mix_Chunk *gVictory;
Mix_Chunk *gWarpPipe;
Mix_Chunk *gHealth;
Mix_Chunk *gFireball;

// ------------------------------------------------------------------


// The keypress handler, or NULL if none has been configured.
key_handler_t key_handler = NULL;

// SDL's timestamp when a key was last pressed or released.
// Used to measure how long a key has been held.
uint32_t key_start_timestamp;

// The value of clock() when time_since_last_tick() was last called.
// Initially 0.
clock_t last_clock = 0;


// Converts an SDL key code to a char.
// 7-bit ASCII characters are just returned and arrow keys are given special character codes.
char get_keycode(SDL_Keycode key) 
{
    switch (key) 
    {
        case SDLK_LEFT:  return LEFT_ARROW;
        case SDLK_UP:    return UP_ARROW;
        case SDLK_RIGHT: return RIGHT_ARROW;
        case SDLK_DOWN:  return DOWN_ARROW;
        case SDLK_ESCAPE: return ESCAPE_KEY;
        case SDLK_RETURN: return RETURN_KEY;
        default:
            // Only process 7-bit ASCII characters.
            //
            // NOTE: This reads like "If casting the keycode to a char and then 
            // back to an SDL_Keycode is the same as the original SDL_Keycode 
            // (i.e. if the keycode is already in ASCII), then return the 
            // keycode. Else, return an empty string.
            return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

void load_audio()
{
    //Load music
    gMenuMusic = Mix_LoadMUS("resources/Audio/mario_08.mp3");
    if(gMenuMusic == NULL) {printf( "Failed to load mario music! SDL_mixer Error: %s\n", Mix_GetError());}
    gToken = Mix_LoadWAV("resources/Audio/mb_coin.wav");
    if (gToken == NULL) {printf( "Failed to load token sfx! SDL_mixer Error: %s\n", Mix_GetError());}
    gJump = Mix_LoadWAV("resources/Audio/smb2_jump.wav");
    if (gJump == NULL) {printf( "Failed to load token sfx! SDL_mixer Error: %s\n", Mix_GetError());}
    gPowerUp = Mix_LoadWAV("resources/Audio/Powerup.wav");
    if (gPowerUp == NULL) {printf("Failed to load powerup sfx! SDL_mixer Error: %s\n", Mix_GetError());}
    gDeath = Mix_LoadWAV("resources/Audio/GameOver.wav");
    if (gDeath == NULL) {printf("Failed to load GameOver sfx! SDL_mixer Error: %s\n", Mix_GetError());}
    gVictory = Mix_LoadWAV("resources/Audio/Stage_clear.wav");
    if (gVictory == NULL) {printf("Failed to load Stage_clear sfx! SDL_mixer Error: %s\n", Mix_GetError());}
    gWarpPipe = Mix_LoadWAV("resources/Audio/WarpPipe.wav");
    if (gWarpPipe == NULL) {printf("Failed to load WarpPipe sfx! SDL_mixer Error: %s\n", Mix_GetError());}
    gHealth = Mix_LoadWAV("resources/Audio/smw_1-up.wav");
    if (gHealth == NULL) {printf("Failed to load health sfx! SDL_mixer Error: %s\n", Mix_GetError());}
    gFireball = Mix_LoadWAV("resources/Audio/FireBall.wav");
    if (gFireball == NULL) {printf("Failed to load fireball sfx! SDL_mixer Error: %s\n", Mix_GetError());}
}

// Initialize an SDL window.
void sdl_init(vector_t min, vector_t max) 
{
    // Check parameters.
    assert(min.x < max.x);
    assert(min.y < max.y);
   
    // SDL_INIT_EVERYTHING initializes timer, audio, haptic, video, joystick, 
    // game controller, and events.
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {printf("SDL initialization successful.\n");}
    else {printf("Initialization failed: %s\n", SDL_GetError());}
    
    if (TTF_Init() == 0) {printf("TTF initialization successful.\n");}
    else {printf("Initialization failed: %s\n", SDL_GetError());}

    window_info = malloc(sizeof(window_info_t));
    assert(window_info != NULL);
    // Initializes the window and stores info in the window_info struct. 
    if (window_info_init(min, max, window_info) == 0) {printf("Window initialization successful.\n");}
    else {printf("Initialization failed: %s\n", SDL_GetError());}
    
    // Initializes the renderer.
    assert(window_info -> window != NULL);
    renderer = malloc(sizeof(SDL_Renderer*));
    renderer = SDL_CreateRenderer(window_info -> window, -1, 0);
    if (renderer != NULL) {printf("Renderer initialization successful.\n");}
    else {printf("Initialization failed: %s\n", SDL_GetError());}

    // Loads the textures to the textures list.
    textures = list_init(NUM_ROLES + NUM_LEVELS, SDL_DestroyTexture);
    frames = list_init(NUM_ROLES + NUM_LEVELS, NULL);
    assert(textures != NULL && frames != NULL);
    if (sdl_textures_init(textures, frames, renderer) == 0) {printf("Texture initialization successful.\n");}
    else {printf("Initialization failed: %s\n", SDL_GetError());}
    
    // Why doesn't passing the pointers into a function work (like what I had 
    // before)?
    TITLE_FONT = TTF_OpenFont("resources/fonts/title.ttf", 25);
    REGULAR_FONT = TTF_OpenFont("resources/fonts/title.ttf", 15);
    if (TITLE_FONT != NULL && REGULAR_FONT != NULL) {printf("Font initialization successful.\n");}
    else {printf("Initialization failed: %s\n", SDL_GetError());}

    if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {printf("SDL_mixer could not initialzie!: %s,\n", Mix_GetError());} 
    else {printf("Audio initialization successful.\n");}
    load_audio();
} 

void sdl_cleanup()
{
    // Free textures
    list_free(textures);
    textures = NULL;

    // Free Music
    Mix_FreeMusic(gMenuMusic);
    Mix_FreeChunk(gToken);
    Mix_FreeChunk(gJump);
    Mix_FreeChunk(gPowerUp);
    Mix_FreeChunk(gDeath);
    Mix_FreeChunk(gVictory);
    Mix_FreeChunk(gWarpPipe);
    Mix_FreeChunk(gHealth);
    Mix_FreeChunk(gFireball);

    gMenuMusic = NULL;
    gLevelMusic = NULL;
    gToken = NULL;
    gJump = NULL;
    gPowerUp = NULL;
    gDeath = NULL;
    gVictory = NULL;
    gWarpPipe = NULL;
    gHealth = NULL;
    gFireball = NULL;

    SDL_DestroyRenderer(renderer);
    window_info_free(window_info);
    renderer = NULL;
    window_info = NULL;

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

// Checks whether or not the user has tried to exit the SDL window.
bool sdl_is_done(void* passer) 
{
    SDL_Event *event = malloc(sizeof(SDL_Event));
    assert(event != NULL);

    while (SDL_PollEvent(event)) 
    {
        switch (event -> type) 
        {
            case SDL_QUIT:
                free(event);
                return true;
            
            case SDL_KEYDOWN:
            // An SDL_KEYUP event occurs when the user stops pressing a key.
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured or an unrecognized key was pressed
                if (key_handler == NULL) break;
                char key = get_keycode(event -> key.keysym.sym);
                if (key == '\0') break;

                uint32_t timestamp = event -> key.timestamp;

                if (!event -> key.repeat) {key_start_timestamp = timestamp;}

                key_event_type_t type = event -> type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                
                key_handler(key, type, held_time, passer);
                
                break;
        }
    }
    free(event);
    return false;
}

// Checks whether or not the user has tried to exit the SDL window.
bool sdl_is_done_testing(void* passer) 
{
    SDL_Event *event = malloc(sizeof(SDL_Event));
    assert(event != NULL);

    while (SDL_PollEvent(event)) 
    {
        switch (event -> type) 
        {
            case SDL_QUIT:
                free(event);
                return true;
            
            case SDL_KEYDOWN:
            // An SDL_KEYUP event occurs when the user stops pressing a key.
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured or an unrecognized key was pressed
                if (key_handler == NULL) break;
                char key = get_keycode(event -> key.keysym.sym);
                if (key == '\0') break;

                uint32_t timestamp = event -> key.timestamp;

                if (!event -> key.repeat) {key_start_timestamp = timestamp;}

                key_event_type_t type = event -> type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                
                key_handler(key, type, held_time, passer);
                
                break;
        }
    }
    free(event);
    return false;
}


// Clears the SDL window.
void sdl_clear() 
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}


// Clears the SDL window, and redraws it; this is to be called each frame.
void sdl_render_scene(scene_t *scene) 
{
    for (size_t role = 0; role < NUM_ROLES; role ++) 
    {
        list_t* role_list = (list_t*) scene_get_list(scene, role);
        for (size_t i = 0; i < list_size(role_list); i ++)
        {
            body_t* body = list_get(role_list, i);
            sdl_draw_sprite(body, textures, frames, renderer, *window_info);
        }
    }
   
    // NOTE: From the SDL documentation, "SDL's rendering functions operate on a backbuffer; 
    // that is, calling a rendering function such as SDL_RenderDrawLine() does not directly 
    // put a line on the screen, but rather updates the backbuffer." 
    // SDL_RenderPresent(renderer);
    // sdl_clear();
}

// Clears the SDL window, and renders menu
void sdl_render() 
{
    // NOTE: From the SDL documentation, "SDL's rendering functions operate on a backbuffer; 
    // that is, calling a rendering function such as SDL_RenderDrawLine() does not directly 
    // put a line on the screen, but rather updates the backbuffer." 
    SDL_RenderPresent(renderer);
    sdl_clear();
}

// This sets the key handler function.
void sdl_on_key(key_handler_t handler) 
{
    key_handler = handler;
}


// Finds the time elapsed in the runtime of the program since the last time 
// this function was called.
double time_since_last_tick(void) 
{
    // NOTE: From the C library documentation, "clock_t clock(void) returns the number of 
    // clock ticks elapsed since the program was launched. To get the number of seconds 
    // used by the CPU, you will need to divide by CLOCKS_PER_SEC."
    clock_t now = clock();
    // If last_clock is not zero, i.e. registers as a "true" bolean, then 
    // define the difference (in seconds); else, define the difference to be 
    // zero.
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;

    return difference;
}


// Text rendering -----------------------------------------------

void sdl_write_title(char* string, rgb_color_t color, vector_t position)
{
    sdl_write(TITLE_FONT, string, color, position, renderer, *window_info);
}

void sdl_write_regular(char* string, rgb_color_t color, vector_t position)
{    
    sdl_write(REGULAR_FONT, string, color, position, renderer, *window_info);
}

