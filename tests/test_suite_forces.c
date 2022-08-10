#include "forces.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include "vector.h"

shape_t *make_shape() 
{
    list_t* vertices = list_init(4, free);

    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t) {-1, -1};
    list_add(vertices, v);

    v = malloc(sizeof(*v));
    *v = (vector_t) {+1, -1};
    list_add(vertices, v);

    v = malloc(sizeof(*v));
    *v = (vector_t) {+1, +1};
    list_add(vertices, v);

    v = malloc(sizeof(*v));
    *v = (vector_t) {-1, +1};
    list_add(vertices, v);
    
    shape_t* shape = shape_init_polygon(vertices);
    return shape;
}


// Tests that a mass on a spring oscillates like A cos(sqrt(K / M) * t)
void test_spring_sinusoid() 
{
    const double M = 10;
    const double K = 2;
    const double A = 3;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *mass = body_init(make_shape(), M, (rgb_color_t) {0, 0, 0});
    body_set_centroid(mass, (vector_t) {A, 0});
    scene_add_body(scene, mass, 0);
    body_t *anchor = body_init(make_shape(), INFINITY, (rgb_color_t) {0, 0, 0});
    scene_add_body(scene, anchor, 0);
    create_spring(scene, K, mass, anchor);
    for (int i = 0; i < STEPS; i++) {
        assert(vec_isclose(
            body_get_centroid(mass),
            (vector_t) {A * cos(sqrt(K / M) * i * DT), 0}
        ));
        assert(vec_equal(body_get_centroid(anchor), VEC_ZERO));
        scene_tick(scene, DT);
    }
    scene_free(scene);
}

double gravity_potential(double G, body_t *body1, body_t *body2) {
    vector_t r = vec_subtract(body_get_centroid(body2), body_get_centroid(body1));
    return -G * body_get_mass(body1) * body_get_mass(body2) / sqrt(vec_dot(r, r));
}
double kinetic_energy(body_t *body) {
    vector_t v = body_get_velocity(body);
    return body_get_mass(body) * vec_dot(v, v) / 2;
}

// Tests that a conservative force (gravity) conserves K + U
void test_energy_conservation() {
    const double M1 = 4.5, M2 = 7.3;
    const double G = 1e3;
    const double DT = 1e-6;
    const int STEPS = 1000000;
    scene_t *scene = scene_init();
    body_t *mass1 = body_init(make_shape(), M1, (rgb_color_t) {0, 0, 0});
    scene_add_body(scene, mass1, 0);
    body_t *mass2 = body_init(make_shape(), M2, (rgb_color_t) {0, 0, 0});
    body_set_centroid(mass2, (vector_t) {10, 20});
    scene_add_body(scene, mass2, 0);
    // create_newtonian_gravity(scene, G, mass1, mass2);
    double initial_energy = gravity_potential(G, mass1, mass2);
    for (int i = 0; i < STEPS; i++) {
        assert(body_get_centroid(mass1).x < body_get_centroid(mass2).x);
        double energy = gravity_potential(G, mass1, mass2) +
            kinetic_energy(mass1) + kinetic_energy(mass2);
        assert(within(1e-4, energy / initial_energy, 1));
        scene_tick(scene, DT);
    }
    scene_free(scene);
}

body_t *make_triangle_body() 
{
    list_t *vertices = list_init(3, free);
    vector_t *v1 = malloc(sizeof(*v1));
    *v1 = (vector_t) {1, 0};
    list_add(vertices, v1);
    vector_t *v2 = malloc(sizeof(*v2));
    *v2 = (vector_t) {-0.5, +sqrt(3) / 2};
    list_add(vertices, v2);
    vector_t *v3 = malloc(sizeof(*v3));
    *v3 = (vector_t) {-0.5, -sqrt(3) / 2};
    list_add(vertices, v3);

    shape_t* shape = shape_init_polygon(vertices);
    return body_init(shape, 1, (rgb_color_t) {0, 0, 0});
}

// Tests that destructive collisions remove lists from the scene
void test_collisions() {
    const double DT = 0.1;
    const double V = 1.23;
    const double SEPARATION_AT_COLLISION = 1.5;
    const int TICKS_TO_COLLISION = 10;

    scene_t *scene = scene_init();
    body_t *body1 = make_triangle_body();
    vector_t initial_separation =
        {SEPARATION_AT_COLLISION + V * DT * (TICKS_TO_COLLISION - 0.5), 0};
    body_set_centroid(body1, vec_negate(initial_separation));
    body_set_velocity(body1, (vector_t) {+V, 0});
    scene_add_body(scene, body1, 0);

    body_t *body2 = make_triangle_body();
    scene_add_body(scene, body2, 0);

    body_t *body3 = make_triangle_body();
    body_set_velocity(body3, (vector_t) {-V, 0});
    body_set_centroid(body3, initial_separation);
    scene_add_body(scene, body3, 0);

    create_destructive_collision(scene, body1, body2);
    create_destructive_collision(scene, body1, body3);
    create_destructive_collision(scene, body2, body3);
    for (int i = 0; i < TICKS_TO_COLLISION * 2; i++) {
        scene_tick(scene, DT);
        // Before collision, there should be 2 lists; after, there should be 0
        if (i < TICKS_TO_COLLISION) {
            assert(list_size(scene_get_list(scene, 0)) == 3);
        }
        else {
            assert(list_size(scene_get_list(scene, 0)) == 0);
        }
    }
    assert(list_size(scene_get_list(scene, 0)) == 0);
    scene_free(scene);
}

// Tests that force creators properly register their list of affected lists.
// If they don't, asan will report a heap-use-after-free failure.
void test_forces_removed() {
    scene_t *scene = scene_init();
    for (int i = 0; i < 10; i++) {
        body_t *body = body_init(make_shape(), 1, (rgb_color_t) {0, 0, 0});
        body_set_centroid(body, (vector_t) {i, i});
        scene_add_body(scene, body, 0);
        for (int j = 0; j < i; j++) {
            // create_newtonian_gravity(scene, 1, body, (body_t*) scene_get_body(scene, j, 0));
            create_spring(scene, 1, body, (body_t*) scene_get_body(scene, j, 0));
        }
        create_drag(scene, 1, body);
    }
    while (list_size(scene_get_list(scene, 0)) > 0) {
        scene_remove_body(scene, 0, 0);
        scene_tick(scene, 1);
    }
    scene_free(scene);
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_spring_sinusoid)
    DO_TEST(test_energy_conservation)
    DO_TEST(test_collisions)
    DO_TEST(test_forces_removed)

    puts("forces_test PASS");
}
