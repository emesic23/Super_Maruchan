#include "shape.h"
#include "test_util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

// Make square at (+/-1, +/-1)
shape_t *make_square() 
{
    list_t* vertices = list_init(4, free);

    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t) {+1, +1};
    list_add(vertices, v);

    v = malloc(sizeof(*v));
    *v = (vector_t) {-1, +1};
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {-1, -1};
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {+1, -1};
    list_add(vertices, v);
    
    shape_t *sq = shape_init_polygon(vertices);
    return sq;
}


void test_square_area_centroid() 
{
    shape_t *sq = make_square();
    
    assert(isclose(shape_area(sq), 4));
    assert(vec_isclose(shape_centroid(sq), VEC_ZERO));

    shape_free(sq);
}


void test_square_translate() 
{
    shape_t *sq = make_square();
    shape_translate(sq, (vector_t) {2, 3});

    list_t* vertices = shape_vertices(sq);

    vector_t* v0 = list_get(vertices, 0);
    vector_t* v1 = list_get(vertices, 1);
    vector_t* v2 = list_get(vertices, 2);
    vector_t* v3 = list_get(vertices, 3);

    assert(vec_equal(*v0, (vector_t) {3, 4}));
    assert(vec_equal(*v1, (vector_t) {1, 4}));
    assert(vec_equal(*v2, (vector_t) {1, 2}));
    assert(vec_equal(*v3, (vector_t) {3, 2}));

    assert(isclose(shape_area(sq), 4));
    assert(vec_isclose(shape_centroid(sq), (vector_t) {2, 3}));
    
    shape_free(sq);
}


void test_square_rotate() {
    shape_t *sq = make_square();
    shape_rotate(sq, 0.25 * M_PI, VEC_ZERO);

    list_t* vertices = shape_vertices(sq);
    
    vector_t* v0 = list_get(vertices, 0);
    vector_t* v1 = list_get(vertices, 1);
    vector_t* v2 = list_get(vertices, 2);
    vector_t* v3 = list_get(vertices, 3);

    assert(vec_isclose(*v0, (vector_t) {0, sqrt(2)}));
    assert(vec_isclose(*v1, (vector_t) {-sqrt(2), 0}));
    assert(vec_isclose(*v2, (vector_t) {0, -sqrt(2)}));
    assert(vec_isclose(*v3, (vector_t) {sqrt(2), 0}));

    assert(isclose(shape_area(sq), 4));
    assert(vec_isclose(shape_centroid(sq), VEC_ZERO));
    
    shape_free(sq);
}


// Make 3-4-5 triangle
shape_t *make_triangle() 
{
    list_t* vertices = list_init(3, (free_func_t) free);

    vector_t *v = malloc(sizeof(*v));
    *v = VEC_ZERO;
    list_add(vertices, v);

    v = malloc(sizeof(*v));
    *v = (vector_t) {4, 0};
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {4, 3};
    list_add(vertices, v);

    shape_t* tri = shape_init_polygon(vertices);
    return tri;
}


void test_triangle_area_centroid() 
{
    shape_t *tri = make_triangle();
    assert(isclose(shape_area(tri), 6));
    assert(vec_isclose(shape_centroid(tri), (vector_t) {8.0 / 3.0, 1}));

    shape_free(tri);
}


void test_triangle_translate() 
{
    shape_t *tri = make_triangle();
    shape_translate(tri, (vector_t) {-4, -3});
    
    list_t* vertices = shape_vertices(tri);

    vector_t* v0 = list_get(vertices, 0);
    vector_t* v1 = list_get(vertices, 1);
    vector_t* v2 = list_get(vertices, 2);

    assert(vec_equal(*v0, (vector_t) {-4, -3}));
    assert(vec_equal(*v1, (vector_t) {0,  -3}));
    assert(vec_equal(*v2, (vector_t) {0,  0}));

    assert(isclose(shape_area(tri), 6));
    assert(vec_isclose(shape_centroid(tri), (vector_t) {-4.0 / 3.0, -2}));

    shape_free(tri);
}


void test_triangle_rotate() {
    shape_t *tri = make_triangle();
    // Rotate -acos(4/5) degrees around (4,3)
    shape_rotate(tri, -acos(4.0 / 5.0), (vector_t) {4, 3});

    list_t* vertices = shape_vertices(tri);

    vector_t* v0 = list_get(vertices, 0);
    vector_t* v1 = list_get(vertices, 1);
    vector_t* v2 = list_get(vertices, 2);

    assert(vec_isclose(*v0, (vector_t) {-1,  3}));
    assert(vec_isclose(*v1, (vector_t) {2.2, 0.6}));
    assert(vec_isclose(*v2, (vector_t) {4,   3}));

    assert(isclose(shape_area(tri), 6));
    assert(vec_isclose(shape_centroid(tri), (vector_t) {26.0 / 15.0, 2.2}));

    shape_free(tri);
}

#define CIRC_NPOINTS 1000000
#define CIRC_AREA (CIRC_NPOINTS * sin(2 * M_PI / CIRC_NPOINTS) / 2)

// Circle with many points (stress test)
shape_t *make_big_circ() 
{
    list_t* vertices = list_init(CIRC_NPOINTS, free);

    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        vector_t *v = malloc(sizeof(*v));
        *v = (vector_t) {cos(angle), sin(angle)};
        list_add(vertices, v);
    }

    shape_t* c = shape_init_polygon(vertices);
    
    return c;
}


void test_circ_area_centroid() 
{
    shape_t *c = make_big_circ();
    assert(isclose(shape_area(c), CIRC_AREA));
    assert(vec_isclose(shape_centroid(c), VEC_ZERO));
    
    shape_free(c);
}


void test_circ_translate() 
{
    shape_t *c = make_big_circ();
    shape_translate(c, (vector_t) {100, 200});
    
    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        vector_t* v = list_get(shape_vertices(c), i);
        assert(vec_isclose(*v,
          (vector_t) {100 + cos(angle), 200 + sin(angle)}));
    }
    assert(isclose(shape_area(c), CIRC_AREA));
    assert(vec_isclose(shape_centroid(c), (vector_t) {100, 200}));

    shape_free(c);
}

void test_circ_rotate() {
    // Rotate about the origin at an unusual angle
    const double ROT_ANGLE = 0.5;

    shape_t *c = make_big_circ();
    shape_rotate(c, ROT_ANGLE, VEC_ZERO);

    for (size_t i = 0; i < CIRC_NPOINTS; i++) {
        double angle = 2 * M_PI * i / CIRC_NPOINTS;
        vector_t* v = list_get(shape_vertices(c), i);
        assert(vec_isclose(
            *v,
            (vector_t) {cos(angle + ROT_ANGLE), sin(angle + ROT_ANGLE)}
        ));
    }
    assert(isclose(shape_area(c), CIRC_AREA));
    assert(vec_isclose(shape_centroid(c), VEC_ZERO));

    shape_free(c);
}


// Weird nonconvex shape.
shape_t *make_weird() 
{
    list_t* vertices = list_init(5, (free_func_t) free);
    
    vector_t *v = malloc(sizeof(*v));
    *v = VEC_ZERO;
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {4, 1};
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {-2, 1};
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {-5, 5};
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {-1, -8};
    list_add(vertices, v);
    
    shape_t* w = shape_init_polygon(vertices);
    return w;
}


void test_weird_area_centroid() 
{
    shape_t *w = make_weird();

    assert(isclose(shape_area(w), 23));
    assert(vec_isclose(shape_centroid(w), (vector_t) {-223.0 / 138.0, -51.0 / 46.0}));

    shape_free(w);
}


void test_weird_translate() {
    shape_t *w = make_weird();
    
    shape_translate(w, (vector_t) {-10, -20});

    list_t* vertices = shape_vertices(w);

    vector_t* v0 = list_get(vertices, 0);
    vector_t* v1 = list_get(vertices, 1);
    vector_t* v2 = list_get(vertices, 2);
    vector_t* v3 = list_get(vertices, 3);
    vector_t* v4 = list_get(vertices, 4);

    assert(vec_isclose(*v0, (vector_t) {-10, -20}));
    assert(vec_isclose(*v1, (vector_t) {-6,  -19}));
    assert(vec_isclose(*v2, (vector_t) {-12, -19}));
    assert(vec_isclose(*v3, (vector_t) {-15, -15}));
    assert(vec_isclose(*v4, (vector_t) {-11, -28}));

    assert(isclose(shape_area(w), 23));
    assert(vec_isclose(shape_centroid(w), (vector_t) {-1603.0 / 138.0, -971.0 / 46.0}));

    shape_free(w);
}


void test_weird_rotate() 
{
    shape_t *w = make_weird();
    // Rotate 90 degrees around (0, 2)
    shape_rotate(w, M_PI / 2, (vector_t) {0, 2});
    
    list_t* vertices = shape_vertices(w);

    vector_t* v0 = list_get(vertices, 0);
    vector_t* v1 = list_get(vertices, 1);
    vector_t* v2 = list_get(vertices, 2);
    vector_t* v3 = list_get(vertices, 3);
    vector_t* v4 = list_get(vertices, 4);

    assert(vec_isclose(*v0, (vector_t) {2,  2}));
    assert(vec_isclose(*v1, (vector_t) {1,  6}));
    assert(vec_isclose(*v2, (vector_t) {1,  0}));
    assert(vec_isclose(*v3, (vector_t) {-3, -3}));
    assert(vec_isclose(*v4, (vector_t) {10, 1}));

    assert(isclose(shape_area(w), 23));
    assert(vec_isclose(shape_centroid(w), (vector_t) {143.0 / 46.0, 53.0 / 138.0}));

    shape_free(w);
}

int main(int argc, char *argv[]) {
    // Run all tests? True if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_square_area_centroid)
    DO_TEST(test_square_translate)
    DO_TEST(test_square_rotate)
    DO_TEST(test_triangle_area_centroid)
    DO_TEST(test_triangle_translate)
    DO_TEST(test_triangle_rotate)
    DO_TEST(test_circ_area_centroid)
    DO_TEST(test_circ_translate)
    DO_TEST(test_circ_rotate)
    DO_TEST(test_weird_area_centroid)
    DO_TEST(test_weird_translate)
    DO_TEST(test_weird_rotate)

    puts("polygon_test PASS");
}
