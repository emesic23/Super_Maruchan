#include "sdl_draw.h"


// Takes a texture, the role associated with the texture, and the list of
// frames as input. This basically just initializes the size of a frame for
// each sprite sheet.
void sdl_frames_init(SDL_Texture* texture, role_t role, list_t* frames)
{
    int total_width;
    int total_height;
    // Note that the dimensions will be kept in terms of pixels (hence the int
    // type).
    SDL_QueryTexture(texture, NULL, NULL, &total_width, &total_height);

    // Initializes the frame dimensions of the sheet (by default, there is one
    // frame in a sheet, for non-animated sprites).
    vector_t sheet = (vector_t) {1, 1};
    if (role == PLAYER) {sheet = PLAYER_SHEET;}
    else if (role == ENEMY) {sheet = ENEMY_SHEET;}
    else if (role == PLATFORM) {sheet = PLATFORM_SHEET;}

    // Break the sprite sheet into rectangles of these dimensions. This
    // assumes all frames are of equal size.
    // REMEMBER, THE ORIGIN IS IN THE TOP LEFT.
    SDL_Rect* frame = malloc(sizeof(SDL_Rect));
    *frame = (SDL_Rect) {0, 0, total_width / sheet.y, total_height / sheet.x};

    list_add(frames, frame);
}


// Takes in an empty list and loads textures from the resources directory into
// the list. Returns 0 if successful and 1 if there is a failure.
int sdl_textures_init(list_t* textures, list_t* frames, SDL_Renderer* renderer)
{
    // Does this need to be freed? I don't think so, stack-allocated.
    char* images[NUM_ROLES + NUM_LEVELS + NUM_CUTSCENES];
    // The order of this array matters! But should be handled by the role_t
    // enum struct in sprite.h.
    images[BACKGROUND] = "resources/mario-1-1.png";
    // Platform is not associated with an image.
    images[PLATFORM] = "resources/platform.png";
    images[POWERUP] = "resources/fireballpowerup.png";
    images[TOKEN] = "resources/token.png";
    images[ENEMY] = "resources/enemy.png";
    images[PLAYER] = "resources/MarioAndLuigi.png";
    images[LEVEL1] = "resources/mario-1-1.png";
    images[LEVEL2] = "resources/1-2.png";
    images[LEVEL2_1] = "resources/mario-1-2.png";
    images[LEVEL3] = "resources/mario-1-3.png";
    images[LEVEL3_1] = "resources/Troll.png";
    images[CUTSCENE1] = "resources/cutscenes/scene1.png";
    images[CUTSCENE2] = "resources/cutscenes/scene2.png";
    images[CUTSCENE3] = "resources/cutscenes/scene3.png";
    images[CUTSCENE4] = "resources/cutscenes/scene4.png";
    images[CUTSCENE5] = "resources/cutscenes/scene5.png";
    images[CUTSCENE6] = "resources/cutscenes/scene6.png";
    images[CUTSCENE7] = "resources/cutscenes/scene7.png";
     images[CUTSCENE8] = "resources/cutscenes/directions.png";

    // Start at 1 and skip background
    for (size_t role = 0; role < (NUM_ROLES + NUM_LEVELS + NUM_CUTSCENES); role ++)
    {
        // Note that if a sprite is not associated with an image, a "blank"
        // texture is loaded to preserve indexing.
        SDL_Surface* surface = IMG_Load(images[role]);
        // Exit if any image couldn't be loaded.
        if (surface == NULL) {return 1;}

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        list_add(textures, texture);
        // Adds the frames corresponding to the texture to the frames list.
        sdl_frames_init(texture, role, frames);

        SDL_FreeSurface(surface);
    }

    return 0;
}


// Text rendering -----------------------------------------------------------------

void sdl_write(TTF_Font* font,
        char* string,
        rgb_color_t color,
        vector_t position,
        SDL_Renderer* renderer,
        window_info_t window_info)
{
    SDL_Color sdl_color = {255 * color.r, 255 * color.g, 255 * color.b};
    // assert(color != NULL);
    assert(string != NULL);
    SDL_Surface* surface = TTF_RenderText_Solid(font, string, sdl_color);
    assert(surface != NULL);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Get the position in pixel coordinates.
    position = get_window_position(position, window_info);

    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    // Make a "destination" rectangle so the text is the correct size and in the
    // correct place.
    SDL_Rect dest_rect = {position.x - width / 2, position.y - height / 2, width, height};
    // Source rectangle is null, as the entire texture is being copied.
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

    SDL_FreeSurface(surface);
}

// Drawing sprites -----------------------------------------------------------------

// Draws a sprite on the window.
void sdl_draw_sprite(body_t* body, list_t* textures, list_t* frames, SDL_Renderer* renderer, window_info_t window_info)
{
    sprite_t* sprite = (sprite_t*) body_get_info(body);
    sprite_state_t state = sprite_get_state(sprite);
    role_t role = sprite_role(sprite);
    subrole_t subrole = sprite_subrole(sprite);
    vector_t current_frame = sprite_current_frame(sprite);
    // Adds position to animate
    int animate_frame = state.start.y;
    SDL_Texture* texture;
    SDL_Rect frame;
    double size = role;
    if (role == BACKGROUND)
    {
        texture = list_get(textures, subrole);
        frame = *(SDL_Rect*) list_get(frames, subrole);
        size = subrole;

    }
    else 
    {
    texture = list_get(textures, size);
    frame = *(SDL_Rect*) list_get(frames, size);
    }

    // shape_t* shape = body_get_shape(body);
    // sdl_draw_shape(shape, (rgb_color_t) {0.5, 0.5, 0.5}, renderer, window_info);
    // shape_free(shape); 
    // returns anny platform that is not item block
    if (subrole != ITEM_BLOCK && role == PLATFORM) {return;}


    SDL_Rect source_rect;
    // Select the source rectangle place in the sprite sheet.
    source_rect.x = frame.x + frame.w * ((size_t) current_frame.y + animate_frame);
    source_rect.y = frame.y + frame.h * ((size_t) current_frame.x);
    source_rect.w = frame.w;
    source_rect.h = frame.h;

    // Note that this position is in pixels.
    vector_t position = get_window_position(body_get_centroid(body), window_info);
    // We will probably want to have a scaling factor for each sprite to make
    // them different sizes. This will likely be stored in the sprite struct.
    SDL_Rect dest_rect = {position.x - (frame.w / 2) + state.adjust.x,
    position.y - (frame.h / 2) + state.adjust.y, frame.w * state.scale.x, frame.h * state.scale.y};

    SDL_RenderCopy(renderer, (SDL_Texture*) list_get(textures, size), &source_rect, &dest_rect);
}



// Draws a shape_t object on the window.
void sdl_draw_shape(shape_t* shape, rgb_color_t color, SDL_Renderer* renderer, window_info_t window_info)
{
    if (shape_type(shape) == CIRCLE) {sdl_draw_circle(shape, color, renderer, window_info);}
    else if (shape_type(shape) == POLYGON) {sdl_draw_polygon(shape, color, renderer, window_info);}
}


// Draws a CIRCLE shape_t object on the window.
void sdl_draw_circle(shape_t* shape, rgb_color_t color, SDL_Renderer* renderer, window_info_t window_info)
{
    // Check parameters.
    assert(shape_type(shape) == CIRCLE);
    assert(shape_radius(shape) > 0);

    vector_t centroid = shape_centroid(shape);
    double radius = shape_radius(shape);
    // Scale the centroid and radius for the window in terms of pixels.
    centroid = get_window_position(centroid, window_info);
    radius *= window_info.scale;

    filledCircleRGBA(renderer, centroid.x, centroid.y, radius, color.r * 255, color.g * 255, color.b * 255, 255);
}


// Draws a POLYGON shape_t object on the window.
void sdl_draw_polygon(shape_t* shape, rgb_color_t color, SDL_Renderer* renderer, window_info_t window_info)
{
    list_t* vertices = shape_vertices(shape);

    // Check parameters.
    assert(shape_type(shape) == POLYGON);
    size_t n = list_size(vertices);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    // Convert each vertex to a point on screen.
    // x_points and y_points are simple integer arrays of n elements.
    int16_t *x_vertices = malloc(sizeof(int16_t) * n);
    int16_t *y_vertices = malloc(sizeof(int16_t) * n);
    assert(x_vertices != NULL);
    assert(y_vertices != NULL);

    for (size_t i = 0; i < n; i++)
    {
        // Populate x_points and y_points.
        vector_t *vertex = list_get(vertices, i);

        // Gets the position of the vertex in terms of the pixel coordinates.
        vector_t pixel = get_window_position(*vertex, window_info);
        x_vertices[i] = pixel.x;
        y_vertices[i] = pixel.y;
    }

    // Draw polygon with the given color.
    //
    // NOTE: The final parameter seems to control the opacity. We should add
    // functionality to color.h so that we can adjust this.
    filledPolygonRGBA(renderer, x_vertices, y_vertices, n, color.r * 255, color.g * 255, color.b * 255, 255);

    free(x_vertices);
    free(y_vertices);
}



