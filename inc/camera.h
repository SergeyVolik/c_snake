#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include "flecs.h"
#include "linmath.h"

typedef struct CameraSetting
{
	int persMode;

	float orthoSize;

	float fov;

	float nearPlane;
	float farPlane;

} CameraSetting;

typedef struct CameraViewProj
{
	mat4x4 view;
	mat4x4 proj;

} CameraViewProj;

ecs_entity_t camera_create(ecs_world_t *world, CameraSetting setting);
ecs_entity_t init_camera(ecs_world_t* world);

#endif