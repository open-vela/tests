#ifndef _SQUARELINE_PROJECT_WATCH_DEMO_H
#define _SQUARELINE_PROJECT_WATCH_DEMO_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"
#include "animengine/anim_physics.h"

// void anim_physics_watch(char* info[], int size, void* param);
int anim_physics_engine_test(int argc, char *agrv[]);
int anim_physics_gravity_test(int argc, char *agrv[]);
int anim_physics_body_test(int argc, char *agrv[]);
int anim_physics_distance_joint_test(int argc, char *agrv[]);
int anim_physics_collision_test(int argc, char *agrv[]);
int anim_physics_material_test(int argc, char *agrv[]);
int anim_physics_restitution_test(int argc, char *agrv[]);
int anim_physics_friction_test(int argc, char *agrv[]);
int anim_physics_material_test(int argc, char *agrv[]);
int anim_physics_material_test2(int argc, char *agrv[]);
int anim_physics_first_material(int argc, char *agrv[]);
int anim_physics_revolute_joint_test(int argc, char *agrv[]);
int anim_physics_create_test(int argc, char *agrv[]);
//int anim_physics_revolute_joint_jlx(int argc, char *agrv[])

extern anim_physics_material_t vMaterial;
#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif