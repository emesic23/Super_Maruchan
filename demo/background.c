#include "color.h"
#include "body.h"
#include "scene.h"
#include "vector.h"
#include "shape.h"
#include "initialize.h"
#include "sdl_wrapper.h"
#include "controls.h"
#include "sprite.h"

#include <math.h>
#include <stdio.h>
#include <time.h>

// the dimensions for the sdl window.
const vector_t MAX3 = {1000, 500};
const vector_t MIN3 = {0, 0};


void level_gen(scene_t *scene)
{
    vector_t m = (vector_t) {3000, 500};

    // add_background(scene, m);
    // flor
    add_platform(scene, (vector_t) {m.x/2, 15}, (vector_t) {m.x, 30}, REGULAR_BLOCK, NULL);
    // roof
    add_platform(scene, (vector_t) {m.x/2, m.y}, (vector_t) {m.x, 30}, REGULAR_BLOCK, NULL);
    // leftWall, rightWall
    add_platform(scene, (vector_t) {0, m.y/2}, (vector_t) {30, m.y}, REGULAR_BLOCK, NULL);
    add_platform(scene, (vector_t) {m.x, m.y/2}, (vector_t) {30, m.y}, REGULAR_BLOCK, NULL);
}


void move_camera(scene_t *scene, double x, double y)
{
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

void track_player(scene_t *scene)
{
    body_t *player = scene_get_body(scene, PLAYER, 0);
    vector_t playerPos = body_get_centroid(player);
    list_t *platform_list = scene_get_list(scene, PLATFORM);
    double leftBound = body_get_centroid(list_get(platform_list, 2)).x;
    double rightBound = body_get_centroid(list_get(platform_list, 3)).x;
    bool moved = false;
    if (leftBound < 0 && playerPos.x < 250) {
        move_camera(scene, 250 - playerPos.x, 0);
    }
    if (rightBound > 1000 && playerPos.x > 750) {
        move_camera(scene, 750 - playerPos.x, 0);
    }
    /*// try to avoid calling same function again*/
    /*// leftBound = body_get_centroid(list_get(scene_get_list(scene, PLATFORM), 2)).x;*/
    /*// rightBound = body_get_centroid(list_get(scene_get_list(scene, PLATFORM), 3)).x;*/
    if (leftBound > 0)
    {
        move_camera(scene, 0 - leftBound, 0);
    }
    if (rightBound < 1000)
    {
        move_camera(scene, 1000 - rightBound, 0);
    }
}
// Game loop ------------------------------------------------------------------

int main()
{
    // Seed the random number generator.
    srand(time(NULL));

    // Initialize the scene and sdl window with MIN3imum and MAX3imum dimensions.
    sdl_init(MIN3, MAX3);
    scene_t *scene = scene_init();

    add_player(scene, (vector_t) {200, 200}, PLAYER1);
    add_player(scene, (vector_t) {200, 200}, PLAYER2);
    level_gen(scene);
    // add_platform(scene, (vector_t) {500, 12.5}, (vector_t) {1000, 25});



    add_interactions(scene);
    // NOTE TO JON: MULIPLATER KEYS are in controls.c 
    sdl_on_key((key_handler_t) multiplayer_on_key);

    while (!sdl_is_done_testing(scene))
    {
        // Draw the scene! This function has sdl_show built in.
        sdl_render_scene(scene);
        sdl_render();
        // move_window(-1, 0);
        scene_tick(scene, time_since_last_tick());
        track_player(scene);
    }

    // Free the scene!
    scene_free(scene);
}
