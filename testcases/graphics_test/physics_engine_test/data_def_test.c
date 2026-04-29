#include "data_def_test.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <syslog.h>
#include <stdio.h>
#include "physics_engine_test.h"

ANIMID body_circle, body_circle2, body_circle3;
 

static inline void create_my_body(anim_engine_handle_t instance, physics_node_t* node)
{
    node->body_info.position.x = 232.0f;
    node->body_info.position.y = 100.0f;
    node->body_info.allow_sleep = false;
    node->body_info.type = ANIM_BODY_DYNAMIC;
    node->body = anim_create_body(instance, &(node->body_info));
    anim_set_render_object(instance, node->body, node->obj);
}

//change material
static inline void create_polygon_shape_material1(anim_engine_handle_t instance, physics_node_t *node, float *points, int count)
{
    /*
        @brief: Universal interface for polygonal shapes
    */
    anim_vector2f_t vertixs[8];
    for (size_t i = 0; i < count; i++)
    {
        vertixs[i].x = points[i * 2];
        vertixs[i].y = points[i * 2 + 1];
    }

    ANIMID shape = anim_add_shape_polygon(instance, node->body, vertixs, count);

    node->material.restitution = 0.4f;
    node->material.friction = 0.4f;
    anim_set_shape_material(instance, node->body, shape, &(node->material));
    printf("shape1_material: %f, %f, %f\n", node->material.restitution, node->material.friction, node->material.density);
}

static inline void create_polygon_shape_material2(anim_engine_handle_t instance, physics_node_t *node, float *points, int count)
{
    /*
        @brief: Universal interface for polygonal shapes
    */
    anim_vector2f_t vertixs[8];
    for (size_t i = 0; i < count; i++)
    {
        vertixs[i].x = points[i * 2];
        vertixs[i].y = points[i * 2 + 1];
    }

    ANIMID shape = anim_add_shape_polygon(instance, node->body, vertixs, count);

    node->material.restitution = 0.5f;
    node->material.friction = 0.5f;
    anim_set_shape_material(instance, node->body, shape, &(node->material));
    printf("shape2_material: %f, %f, %f\n", node->material.restitution, node->material.friction, node->material.density);
}
static inline void create_polygon_shape(anim_engine_handle_t instance, physics_node_t *node, float *points, int count)
{
    /*
        @brief: Universal interface for polygonal shapes
    */
    anim_vector2f_t vertixs[8];
    for (size_t i = 0; i < count; i++)
    {
        vertixs[i].x = points[i * 2];
        vertixs[i].y = points[i * 2 + 1];
    }

    ANIMID shape = anim_add_shape_polygon(instance, node->body, vertixs, count);

    node->material.restitution = 0.3f;
    node->material.friction = 0.5f;
    anim_set_shape_material(instance, node->body, shape, &(node->material));
    printf("shape3_material: %f, %f, %f\n", node->material.restitution, node->material.friction, node->material.density);
}

 void create_my_kcal_shape(anim_engine_handle_t instance, physics_node_t* node)
{
    float p1[] = { 36.0400, 55.9440, 12.0400, 67.9440, -49.9600, 43.9440, -55.9600, 30.9440, 1.0400, -69.0560, 44.0400, -27.0560, 56.0400, -0.0560, 56.0400, 23.9440 };
    float p2[] = { -31.9600, 60.9440, -49.9600, 43.9440, 12.0400, 67.9440, -6.9600, 68.9440 };
    float p3[] = { -57.9600, 4.9440, -50.9600, -17.0560, -36.9600, -38.0560, 0.0400, -69.0560, 1.0400, -69.0560, -55.9600, 30.9440 };
    create_my_body(instance, node);

    create_polygon_shape_material1(instance, node, p1, sizeof(p1) / sizeof(float) / 2);
    create_polygon_shape_material2(instance, node, p2, sizeof(p2) / sizeof(float) / 2);
    anim_get_material(instance, node->body, &(node->material));
    printf("get_body_first_shape_material: %f, %f, %f\n", node->material.restitution, node->material.friction, node->material.density);


    create_polygon_shape(instance, node, p3, sizeof(p3) / sizeof(float) / 2);

    

    anim_get_material(instance, node->body, &(node->material));
    printf("get_body_first_shape_material: %f, %f, %f\n", node->material.restitution, node->material.friction, node->material.density);
    node->material.density = 1.0f;
    node->material.friction = 1.0f;
    node->material.restitution = 1.0f;
    anim_set_material(instance, node->body, &(node->material));
    anim_get_material(instance, node->body, &(node->material));
    printf("set_body_first_shape_material: %f, %f, %f\n", node->material.restitution, node->material.friction, node->material.density);
}



static inline ANIMID create_body_DYNAMIC(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create DYNAMIC body
    */
    node->body_info.position.x = 232.0f;
    node->body_info.position.y = 100.0f;
    node->body_info.allow_sleep = false;
    node->body_info.type = ANIM_BODY_DYNAMIC;
    node->body = anim_create_body(instance, &(node->body_info));
    anim_set_render_object(instance, node->body, node->obj);
    return node->body;
}

static inline ANIMID create_body_STATIC(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create STATIC body
    */
    node->body_info.position.x = 332.0f;
    node->body_info.position.y = 100.0f;
    node->body_info.allow_sleep = false;
    node->body_info.type = ANIM_BODY_STATIC;
    node->body = anim_create_body(instance, &(node->body_info));
    anim_set_render_object(instance, node->body, node->obj);
    return node->body;
}
static inline ANIMID create_body_KINEMATIC(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create KINEMATIC body
    */
    node->body_info.position.x = 332.0f;
    node->body_info.position.y = 100.0f;
    node->body_info.allow_sleep = false;
    node->body_info.type = ANIM_BODY_KINEMATIC;
    node->body = anim_create_body(instance, &(node->body_info));
    anim_set_render_object(instance, node->body, node->obj);
    return node->body;
}

static inline ANIMID create_rigid_body_edge_polygon_4_sides(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a pentagram stiffness body, call anim_create_body_polygon
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232;
    info.position.y = 232;

    const int k_segments = 4;                                // Number of sides of regular polygon
    const float r = 20.0f;                                    // Radius of regular polygon
    const float k_increment = (2.0f * 3.14159f) / k_segments; // Radian increment of each angle

    // Define vertex array of regular 4-gon
    anim_vector2f_t pts[4];

    // Calculate vertices of regular 4-gon
    for (int i = 0; i < k_segments; ++i)
    {
        // Calculate x and y coordinates of each vertex
        pts[i].x = cosf(k_increment * i) * r;
        pts[i].y = sinf(k_increment * i) * r;
        printf("Vertex %d: (%.2f, %.2f)\n", i + 1, pts[i].x, pts[i].y);
    }
    ANIMID body = anim_create_body_chain_polygon(instance, &info, pts, 4);
    // node->material.restitution = 0.5f;
    // node->material.friction = 0.6f;
    // anim_set_shape_material(instance, node->body, shape, &(node->material));
    anim_physics_material_t material;
    anim_get_material(instance, body, &material);

    material.density = 1.0f;
    material.restitution = 0.3f;

    anim_set_material(instance, body, &material);
    return node->body;
}

/* --------------------------------------------------------------------------------------------------------------------*/

static inline ANIMID create_circles_shape(anim_engine_handle_t instance, physics_node_t *node, float radius)
{
    /*
        @brief: Universal interface for circular shapes
    */
    ANIMID shape = anim_add_shape_circle(instance, node->body, radius);
    node->material.restitution = 0.5f;
    node->material.friction = 0.6f;
    anim_set_shape_material(instance, node->body, shape, &(node->material));
    return shape;
}






static inline void create_my_shape(anim_engine_handle_t instance, physics_node_t *node, float edge, anim_physics_material_t materials)
{
    /*
        @brief: Universal interface for my shapes
    */
    // anim_vector2f_t vertixs[8];
    // for (size_t i = 0; i < count; i++)
    // {
    //     vertixs[i].x = points[i * 2];
    //     vertixs[i].y = points[i * 2 + 1];
    // }

    // ANIMID shape = anim_add_shape_polygon(instance, node->body, vertixs, count);
    anim_vector2f_t size;
    size.x = edge;
    size.y = edge;
    ANIMID shape = anim_add_shape_box(instance, node->body, &size);
    // // node->material.restitution = 0.3f;
    // // node->material.friction = 0.5f;
    // material.restitution = materials.restitution;
    // material.friction = materials.friction;
    // material.density = 2.0f;
    
    //anim_get_shape_material(instance, node->body, shape, &materials);
    anim_set_shape_material(instance, node->body, shape, &materials);
}

void create_circle_shape1(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a cricle shape
    */
    body_circle = create_body_DYNAMIC(instance, node);
    ANIMID shape = anim_add_shape_circle(instance, node->body, 40.0f);
    node->material.restitution = vMaterial.restitution;;
    node->material.friction = vMaterial.friction;
    node->material.density = vMaterial.density;
    anim_set_shape_material(instance, node->body, shape, &(node->material));
    float pre_material_restitution = node->material.restitution;
    float pre_material_friction = node->material.friction;
    float pre_material_density = node->material.density;
    printf("pre_material_restitution: %f\n", pre_material_restitution);
    printf("pre_material_friction: %f\n", pre_material_friction);
    printf("pre_material_density: %f\n", pre_material_density);
    if(pre_material_density == vMaterial.density&& pre_material_friction == vMaterial.friction && pre_material_restitution == vMaterial.restitution)
    {
        printf("SET_SHAPE_MATERIAL_PASS!\n");
    }
    anim_get_shape_material(instance, node->body, shape, &(node->material));
    float last_material_restitution = node->material.restitution;
    float last_material_friction = node->material.friction;
    float last_material_density = node->material.density;
    printf("last_material_restitution: %f\n", last_material_restitution);
    printf("last_material_friction: %f\n", last_material_friction);
    printf("last_material_density: %f\n", last_material_density);
    if(pre_material_density == last_material_density && pre_material_friction == last_material_friction && pre_material_restitution == last_material_restitution)
    {
        printf("GET_SHAPE_MATERIAL_PASS!\n");
    }
    // printf("get_material_restitution: %f\n", node->material.restitution);
    // printf("get_material_friction: %f\n", node->material.friction);
    // printf("get_material_density: %f\n", node->material.density);

    
    printf("body_circle: %lld\n", body_circle);
    //return shape;
}

// void create_my_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
// {
//     anim_physics_material_t material_test, material_test2;
//     printf("node.body1:%lld\n",node->body);
  
//     node->body = create_rigid_body_edge_polygon_4_sides(instance, node);
//     node->body = create_body_STATIC(instance, node);
//     printf("node.body2:%lld\n",node->body);
//     material_test.restitution = 0.5f;
//     material_test.friction = 0.6f;
//     material_test.density = 2.0f;

//     material_test2.restitution = 0.2f;
//     material_test2.friction = 0.4f;
//     material_test2.density = 1.0f;
//     printf("node.body3:%lld\n",node->body);

//     create_my_shape(instance, node, 15.0f, material_test);
//     printf("node.body_test:%lld\n",node->body);
//     create_my_shape(instance, node, 20.0f, material_test2);
//     printf("node.body_test2:%lld\n",node->body);
// }

/* --------------------------------------------------------------------------------------------------------------------*/
void create_null_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a null stiffness body
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 132;
    info.position.y = 232;
    info.type = ANIM_BODY_STATIC;
    anim_create_body(instance, &info);
}

void create_circle_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a circle stiffness body, call anim_create_body_circle
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 332;
    info.position.y = 232;
    info.type = ANIM_BODY_STATIC;
    ANIMID body = anim_create_body_circle(instance, &info, 60.0f);
    anim_physics_material_t material;
    anim_get_material(instance, body, &material);

    material.density = 2.0f;
    material.restitution = 0.6f;
    anim_set_material(instance, body, &material);

    // ANIMID shape = anim_add_shape_circle(instance, body, 20.0f);
    // node->material.restitution = 0.5f;
    // node->material.friction = 0.6f;
    // anim_set_shape_material(instance, body, shape, &(node->material));
}
void create_box_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a box stiffness body, call anim_create_body_box
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232;
    info.position.y = 232;
    anim_size_t size;
    size.x = 40.0f;
    size.y = 40.0f;
    anim_create_body_box(instance, &info, &size);
}
// create_rigid_body_polygon_17_sides
void create_rigid_body_polygon_17_sides(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a pentagram stiffness body, call anim_create_body_polygon
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232;
    info.position.y = 232;

    const int k_segments = 17;                                // Number of sides of regular polygon
    const float r = 40.0f;                                    // Radius of regular polygon
    const float k_increment = (2.0f * 3.14159f) / k_segments; // Radian increment of each angle

    // Define vertex array of regular 17-gon
    anim_vector2f_t pts[17];

    // Calculate vertices of regular 17-gon
    for (int i = 0; i < k_segments; ++i)
    {
        // Calculate x and y coordinates of each vertex
        pts[i].x = cosf(k_increment * i) * r;
        pts[i].y = sinf(k_increment * i) * r;
        printf("Vertex %d: (%.2f, %.2f)\n", i + 1, pts[i].x, pts[i].y);
    }
    ANIMID body = anim_create_body_polygon(instance, &info, pts, 17);
    // node->material.restitution = 0.5f;
    // node->material.friction = 0.6f;
    // anim_set_shape_material(instance, node->body, shape, &(node->material));
    anim_physics_material_t material;
    anim_get_material(instance, body, &material);

    material.density = 1.0f;
    material.restitution = 0.3f;

    anim_set_material(instance, body, &material);
}
void create_rigid_body_polygon_8_sides(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a pentagram stiffness body, call anim_create_body_polygon
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232;
    info.position.y = 232;

    const int k_segments = 8;
    const float r = 40.0f;
    const float k_increment = (2.0f * 3.14159f) / k_segments;
    // Define vertex array of regular 8-gon
    anim_vector2f_t pts[8];

    // Calculate vertices of regular 8-gon
    for (int i = 0; i < k_segments; ++i)
    {
        // Calculate x and y coordinates of each vertex
        pts[i].x = cosf(k_increment * i) * r;
        pts[i].y = sinf(k_increment * i) * r;
        printf("Vertex %d: (%.2f, %.2f)\n", i + 1, pts[i].x, pts[i].y);
    }
    ANIMID body = anim_create_body_polygon(instance, &info, pts, 8);
    // node->material.restitution = 0.5f;
    // node->material.friction = 0.6f;
    // anim_set_shape_material(instance, node->body, shape, &(node->material));
    anim_physics_material_t material;
    anim_get_material(instance, body, &material);

    material.density = 1.0f;
    material.restitution = 0.3f;

    anim_set_material(instance, body, &material);
}

void create_rigid_body_edge_polygon_17_sides(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a pentagram stiffness body, call anim_create_body_polygon
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232;
    info.position.y = 232;

    const int k_segments = 17;                                // Number of sides of regular polygon
    const float r = 80.0f;                                    // Radius of regular polygon
    const float k_increment = (2.0f * 3.14159f) / k_segments; // Radian increment of each angle

    // Define vertex array of regular 17-gon
    anim_vector2f_t pts[17];

    // Calculate vertices of regular 17-gon
    for (int i = 0; i < k_segments; ++i)
    {
        // Calculate x and y coordinates of each vertex
        pts[i].x = cosf(k_increment * i) * r;
        pts[i].y = sinf(k_increment * i) * r;
        printf("Vertex %d: (%.2f, %.2f)\n", i + 1, pts[i].x, pts[i].y);
    }
    ANIMID body = anim_create_body_chain_polygon(instance, &info, pts, 17);
    // node->material.restitution = 0.5f;
    // node->material.friction = 0.6f;
    // anim_set_shape_material(instance, node->body, shape, &(node->material));
    anim_physics_material_t material;
    anim_get_material(instance, body, &material);

    material.density = 1.0f;
    material.restitution = 0.3f;

    anim_set_material(instance, body, &material);
}

void destory_circle_box_body(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create and destory a box stiffness body
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232;
    info.position.y = 232;
    anim_size_t size;
    size.x = 40.0f;
    size.y = 40.0f;
    ANIMID body = anim_create_body_box(instance, &info, &size);
    anim_destroy_body(instance, body);
}

void create_pentagram_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a pentagram stiffness body, call anim_create_body_polygon
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232;
    info.position.y = 232;

    anim_vector2f_t pts[5];
    const int k_segments = 5;
    const float k_increment = 2.0f * 3.14159f / k_segments;
    float r = 40.0f;
    // quintile circle
    for (int i = 0; i < k_segments; ++i)
    {
        pts[i].x = cosf(-k_increment * i) * r;
        pts[i].y = sinf(-k_increment * i) * r;
        printf("Vertex %d: (%.2f, %.2f)\n", i + 1, pts[i].x, pts[i].y);
    }
    ANIMID body = anim_create_body_polygon(instance, &info, pts, 5);

    anim_physics_material_t material;
    anim_get_material(instance, body, &material);

    material.density = 1.0f;
    material.restitution = 0.3f;

    anim_set_material(instance, body, &material);
}

static void create_week_shape(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a week shape
    */
    float p1[] = {-55.5000, 52.5000, -60.5000, 39.5000, -60.5000, -40.5000, -40.5000, -59.5000, -39.5000, -59.5000, 60.5000, -38.5000, 60.5000, 37.5000, -37.5000, 61.5000};
    float p2[] = {51.5000, 55.5000, 38.5000, 61.5000, -37.5000, 61.5000, 60.5000, 37.5000};
    float p3[] = {-51.5000, -54.5000, -40.5000, -59.5000, -60.5000, -40.5000};

    float p4[] = {54.5000, -50.5000, 60.5000, -38.5000, -39.5000, -59.5000, 40.5000, -59.5000};

    create_body_STATIC(instance, node);
    // create_body_STATIC(instance, node);
    create_polygon_shape(instance, node, p1, sizeof(p1) / sizeof(float) / 2);
    create_polygon_shape(instance, node, p2, sizeof(p2) / sizeof(float) / 2);
    create_polygon_shape(instance, node, p3, sizeof(p3) / sizeof(float) / 2);
    create_polygon_shape(instance, node, p4, sizeof(p4) / sizeof(float) / 2);
    // return body_week;
}

void create_circle_shape(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a cricle shape
    */
    body_circle = create_body_DYNAMIC(instance, node);
    create_circles_shape(instance, node, 40.0f);
    printf("body_circle: %lld\n", body_circle);
}

void create_circle_shape2(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a cricle2 shape
    */
    body_circle2 = create_body_STATIC(instance, node);
    create_circles_shape(instance, node, 40.0f);
    printf("body_circle2: %lld\n", body_circle2);
}

void create_circle_shape3(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a cricle3 shape
    */
    node->body_info.position.x = 232.0f;
    node->body_info.position.y = 100.0f;
    node->body_info.allow_sleep = false;
    node->body_info.type = ANIM_BODY_KINEMATIC;
    node->body = anim_create_body(instance, &(node->body_info));
    anim_set_render_object(instance, node->body, node->obj);
    
    body_circle3 = node->body;
    create_circles_shape(instance, node, 40.0f);
    printf("body_circle3: %lld\n", body_circle3);
}



void destory_shape(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: Create a static empty rigid body, add a shape, and remove one
    */
    create_body_STATIC(instance, node);
    anim_size_t size;
    size.x = 60.0f;
    size.y = 40.0f;
    ANIMID shape = anim_add_shape_box(instance, node->body, &size);
    node->material.restitution = 0.5f;
    node->material.friction = 0.6f;
    anim_set_shape_material(instance, node->body, shape, &(node->material));
    anim_remove_shape(instance, node->body, shape);
}

void create_dynamic_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: create a dynamic box shape
    */
    create_body_DYNAMIC(instance, node);
    anim_size_t size;
    size.x = 60.0f;
    size.y = 40.0f;
    ANIMID shape = anim_add_shape_box(instance, node->body, &size);
    node->material.restitution = 0.5f;
    node->material.friction = 0.6f;
    anim_set_shape_material(instance, node->body, shape, &(node->material));
}
void create_kinematic_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
{
    create_body_KINEMATIC(instance, node);
    anim_size_t size;
    size.x = 40.0f;
    size.y = 40.0f;
    ANIMID shape = anim_add_shape_box(instance, node->body, &size);
    node->material.restitution = 0.0f;
    node->material.friction = 0.0f;
    anim_set_shape_material(instance, node->body, shape, &(node->material));
}

/*material test*/
void create_material1_rigid_body(anim_engine_handle_t instance, physics_node_t *node)
{
    /*
        @brief: Create a square rigid body and add two shapes to it
    */
    anim_body_info_t info;
    anim_body_init(&info);
    info.position.x = 232;
    info.position.y = 232;

    anim_vector2f_t pts[4]; // Square has 4 vertices
    const int k_segments = 4; // Square has 4 vertices
    const float k_increment = 2.0f * 3.14159f / k_segments; // Square has only one perimeter, so no subdivision needed
    float r = 60.0f; // Assume radius is 60

    // Calculate vertices of square
    for (int i = 0; i < k_segments; ++i) {
        // Vertex coordinates of square can be calculated by dividing the radius into two parts
        pts[i].x = cosf(-k_increment * i) * r;
        pts[i].y = sinf(-k_increment * i) * r;
        printf("Vertex %d: (%.2f, %.2f)\n", i + 1, pts[i].x, pts[i].y);
    }
    ANIMID body = anim_create_body_polygon(instance, &info, pts, 4);

    anim_physics_material_t material;
    anim_get_material(instance, body, &material);

    material.density = 1.0f;
    material.restitution = 0.3f;

    anim_set_material(instance, body, &material);
}


