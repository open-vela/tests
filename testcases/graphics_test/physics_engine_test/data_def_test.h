#ifndef __PHYSICS_ENGINE_WATCH_DATA_DEF_H__
#define __PHYSICS_ENGINE_WATCH_DATA_DEF_H__

// #ifndef ANIMENGINE_TEST_H
// #define ANIMENGINE_TEST_H
// #endif

#include "animengine/anim_api.h"
#include "animengine/anim_physics.h"
#include "lvgl.h"

// extern ANIMID body_circle, body_circle2;

struct physics_node_t_;
typedef struct physics_node_t_ physics_node_t;

typedef void (*create_shape_func_t)(anim_engine_handle_t instance, physics_node_t* node);

typedef struct physics_node_t_ {
    lv_obj_t* obj;
    const lv_img_dsc_t* img_src;
    ANIMID body;
    
    create_shape_func_t create_func;

    anim_body_info_t body_info;
    anim_physics_material_t material;
} physics_node_t;

void create_null_rigid_body(anim_engine_handle_t instance, physics_node_t* node);
void create_circle_rigid_body(anim_engine_handle_t instance, physics_node_t* node);
void create_pentagram_rigid_body(anim_engine_handle_t instance, physics_node_t* node);
void create_rigid_body_polygon_8_sides(anim_engine_handle_t instance, physics_node_t* node);
void create_rigid_body_polygon_17_sides(anim_engine_handle_t instance, physics_node_t* node);
void create_rigid_body_edge_polygon_17_sides(anim_engine_handle_t instance, physics_node_t* node);
void create_box_rigid_body(anim_engine_handle_t instance, physics_node_t* node);
void destory_circle_box_body(anim_engine_handle_t instance, physics_node_t* node);
void create_my_rigid_body(anim_engine_handle_t instance, physics_node_t* node);

void create_circle_shape(anim_engine_handle_t instance, physics_node_t* node);
void create_circle_shape1(anim_engine_handle_t instance, physics_node_t *node);
void create_circle_shape2(anim_engine_handle_t instance, physics_node_t* node);
void create_circle_shape3(anim_engine_handle_t instance, physics_node_t* node);
void create_circle_shape4(anim_engine_handle_t instance, physics_node_t* node);
static void create_week_shape(anim_engine_handle_t instance, physics_node_t* node);
void create_box_shape(anim_engine_handle_t instance, physics_node_t* node);
void destory_shape(anim_engine_handle_t instance, physics_node_t* node);


void create_dynamic_rigid_body(anim_engine_handle_t instance, physics_node_t* node);
void create_kinematic_rigid_body(anim_engine_handle_t instance, physics_node_t* node);

void create_material1_rigid_body(anim_engine_handle_t instance, physics_node_t* node);

/*distance joint*/
void create_background_rigid_body(anim_engine_handle_t instance, physics_node_t* node);


//material
void create_my_kcal_shape(anim_engine_handle_t instance, physics_node_t* node);

#endif
