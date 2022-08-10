#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "polygon.h"
#include "vector.h"
#include "body.h"
#include "collision.h"
#include "test_util.h"
#include "shape.h"


rgb_color_t COLOR = (rgb_color_t) {0, 0, 0};
double MASS = 1;


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
    
    shape_t *shape = shape_init_polygon(vertices);
    return shape;
}


shape_t *make_triangle() 
{
    list_t* vertices = list_init(4, free);

    vector_t *v = malloc(sizeof(*v));
    *v = (vector_t) {-1, -1};
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {+1, -1};
    list_add(vertices, v);
    
    v = malloc(sizeof(*v));
    *v = (vector_t) {0, +1};
    list_add(vertices, v);
    
    shape_t *shape = shape_init_polygon(vertices);
    return shape;
}

#define CIRCLE_NPOINTS 1000

shape_t *make_circle()
{
    // Builds circle shape.
    double circle_radius = 1;
    double angle = 0;

    list_t* vertices = list_init(CIRCLE_NPOINTS, free);

    for (size_t i = 0; i < CIRCLE_NPOINTS; i ++)
    {
      vector_t* new_point = malloc(sizeof(vector_t));
      assert(new_point != NULL);

      new_point -> x = circle_radius;
      new_point -> y = 0;
      *new_point = vec_rotate(*new_point, angle, VEC_ZERO);
      list_add(vertices, new_point);
      // Changes the angle of rotation by one increment.
      angle += (2 * M_PI / (CIRCLE_NPOINTS));
    }

    shape_t* shape = shape_init_polygon(vertices);
    return shape;
}


void test_collision_simple()
{
    shape_t *shape1 = make_shape();
    shape_t *shape2 = make_shape();
    body_t *body1 = body_init(shape1, MASS, COLOR);
    body_t *body2 = body_init(shape2, MASS, COLOR);

    assert(find_collision(body1, body2).collided == true);

    shape_t	*temp = body_get_shape(body1);
    shape_translate(temp, (vector_t) {0, 0.5});
    body_set_shape(body1, temp);


    assert(find_collision(body1, body2).collided == true);

    shape_t	*temp1 = body_get_shape(body1);
    shape_translate(temp1, (vector_t) {0, 4});
    body_set_shape(body1, temp1);


    assert(find_collision(body1, body2).collided == false);

    shape_t	*temp2 = body_get_shape(body1);
    shape_translate(temp2, (vector_t) {2, -5});
    body_set_shape(body1, temp2);


    assert(find_collision(body1, body2).collided == true);

    shape_t *temp3 = body_get_shape(body1);
    shape_translate(temp3, (vector_t) {3, 0});
    body_set_shape(body1, temp3);


    assert(find_collision(body1, body2).collided == false);

    shape_t *temp4 = body_get_shape(body1);
    shape_translate(temp4, (vector_t) {-10, 0});
    body_set_shape(body1, temp4);


    assert(find_collision(body1, body2).collided == false);

    body_free(body1);
    body_free(body2);
}

void test_collision_rotation()
{
    shape_t *shape1 = make_shape();
    shape_t *shape2 = make_shape();
    body_t *body1 = body_init(shape1, MASS, COLOR);
    body_t *body2 = body_init(shape2, MASS, COLOR);

    assert(find_collision(body1, body2).collided == true);

    shape_t *temp = body_get_shape(body1);
    shape_rotate(temp, M_PI/4, shape_centroid(temp));
    body_set_shape(body1, temp);


    assert(find_collision(body1, body2).collided == true);

    shape_t *temp1 = body_get_shape(body1);
    shape_translate(temp1, (vector_t) {0, 2.1});
    body_set_shape(body1, temp1);


    assert(find_collision(body1, body2).collided == true);

    shape_t *temp2 = body_get_shape(body1);
    shape_rotate(temp2, -1 * M_PI/4, shape_centroid(temp2));
    body_set_shape(body1, temp2);


    assert(find_collision(body1, body2).collided == false);

    shape_t *temp3 = body_get_shape(body1);
    shape_rotate(temp3, M_PI/2, shape_centroid(temp3));
    shape_translate(temp3, (vector_t) {0, -2.1});
    body_set_shape(body1, temp3);


    assert(find_collision(body1, body2).collided == true);

    body_free(body1);
    body_free(body2);
}

void test_collision_triangle_sq()
{
    shape_t *shape1 = make_triangle();
    shape_t *shape2 = make_shape();
    body_t *body1 = body_init(shape1, MASS, COLOR);
    body_t *body2 = body_init(shape2, MASS, COLOR);

    assert(find_collision(body1, body2).collided == true);

    shape_t *temp = body_get_shape(body1);
    shape_rotate(temp, M_PI/4, shape_centroid(temp));
    body_set_shape(body1, temp);



    assert(find_collision(body1, body2).collided == true);

    shape_t *temp1 = body_get_shape(body1);
    shape_translate(temp1, (vector_t) {0, 2.1});
    body_set_shape(body1, temp1);


    assert(find_collision(body1, body2).collided == true);

    shape_t *temp2 = body_get_shape(body1);
    shape_rotate(temp2, -1 * M_PI/4, shape_centroid(temp1));
    body_set_shape(body1, temp2);



    assert(find_collision(body1, body2).collided == false);

    shape_t *temp3 = body_get_shape(body1);
    shape_translate(temp3, (vector_t) {0, -4.1});
    body_set_shape(body1, temp3);



    assert(find_collision(body1, body2).collided == true);

    shape_t *temp4 = body_get_shape(body1);
    shape_translate(temp4, (vector_t) {0, -0.1});
    body_set_shape(body1, temp4);



    assert(find_collision(body1, body2).collided == false);

    shape_t *temp5 = body_get_shape(body1);
    shape_translate(temp5, (vector_t) {1, 0.1});
    body_set_shape(body1, temp5);



    assert(find_collision(body1, body2).collided == true);

    shape_t *temp6 = body_get_shape(body1);
    shape_translate(temp6, (vector_t) {0.1, 0});
    body_set_shape(body1, temp6);



    assert(find_collision(body1, body2).collided == false);

    shape_t *temp7 = body_get_shape(body1);
    shape_translate(temp7, (vector_t) {0.1, 0.1});
    body_set_shape(body1, temp7);



    assert(find_collision(body1, body2).collided == false);

    shape_t *temp8 = body_get_shape(body1);
    shape_translate(temp8, (vector_t) {0.1, 1});
    body_set_shape(body1, temp8);


    assert(find_collision(body1, body2).collided == true);

    body_free(body1);
    body_free(body2);
}

void test_collision_circle_sq()
{
    shape_t *shape1 = make_circle();
    shape_t *shape2 = make_shape();
    body_t *body1 = body_init(shape1, MASS, COLOR);
    body_t *body2 = body_init(shape2, MASS, COLOR);

    assert(find_collision(body1, body2).collided == true);

    shape_t *temp1 = body_get_shape(body1);
    shape_translate(temp1, (vector_t) {0, 2.1});
    body_set_shape(body1, temp1);
    assert(find_collision(body1, body2).collided == false);


    shape_t *temp2 = body_get_shape(body1);
    shape_translate(temp2, (vector_t) {0, -0.1});
    body_set_shape(body1, temp2);
    assert(find_collision(body1, body2).collided == true);


    shape_t *temp3 = body_get_shape(body1);
    shape_translate(temp3, (vector_t) {0, -2.0});
    body_set_shape(body1, temp3);



    shape_t *temp4 = body_get_shape(body1);
    shape_translate(temp4, (vector_t) {1.5, 2.0});
    body_set_shape(body1, temp4);


    assert(find_collision(body1, body2).collided == false);


    shape_t *temp5 = body_get_shape(body1);
    shape_translate(temp5, (vector_t) {-0.25, -0.5});
    body_set_shape(body1, temp5);
    assert(find_collision(body1, body2).collided == true);

    body_free(body1);
    body_free(body2);
}

void test_collision_circle_triangle()
{
    shape_t *shape1 = make_circle();
    shape_t *shape2 = make_triangle();
    body_t *body1 = body_init(shape1, MASS, COLOR);
    body_t *body2 = body_init(shape2, MASS, COLOR);
    assert(find_collision(body1, body2).collided == true);

    shape_t *temp = body_get_shape(body1);
    shape_translate(temp, (vector_t) {0, 2.1});
    body_set_shape(body1, temp);
    assert(find_collision(body1, body2).collided == false);

    shape_t *temp1 = body_get_shape(body1);
    shape_translate(temp1, (vector_t) {0, -0.101});
    body_set_shape(body1, temp1);

    assert(find_collision(body1, body2).collided == true);


    shape_t *temp2 = body_get_shape(body1);
    shape_translate(temp2, (vector_t) {0, -2.0});
    body_set_shape(body1, temp2);



    shape_t *temp3 = body_get_shape(body1);
    shape_translate(temp3, (vector_t) {1.5, 1.0});
    body_set_shape(body1, temp3);
    assert(find_collision(body1, body2).collided == false);

    shape_t *temp4 = body_get_shape(body1);
    shape_translate(temp4, (vector_t) {-0.25, -0.5});
    body_set_shape(body1, temp4);
    assert(find_collision(body1, body2).collided == true);

    body_free(body1);
    body_free(body2);
}

int main(int argc, char *argv[]) {
    // Run all tests if there are no command-line arguments
    bool all_tests = argc == 1;
    // Read test name from file
    char testname[100];
    if (!all_tests) {
        read_testname(argv[1], testname, sizeof(testname));
    }

    DO_TEST(test_collision_simple)
    DO_TEST(test_collision_rotation)
    DO_TEST(test_collision_triangle_sq)
    DO_TEST(test_collision_circle_sq)
    DO_TEST(test_collision_circle_triangle)

    puts("collision_tests PASS");
}
