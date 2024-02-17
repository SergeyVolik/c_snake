#include "camera.h"
#include "LocalTransform.h"
#include "game_math.h"

ecs_entity_t camera_create(ecs_world_t* world, CameraSetting setting)
{
	ECS_COMPONENT(world, CameraSetting);
	ECS_COMPONENT(world, LocalTransfrom);
	ECS_COMPONENT(world, CameraViewProj);

	ECS_TAG(world, Camera_Tag);

	ecs_entity_t camera_entity = ecs_entity(world, { .name = "Camera" });
	LocalTransfrom trans = transform_default();
	CameraViewProj viewProjC = {0};

	ecs_add_id(world, camera_entity, Camera_Tag);
	ecs_set_id(world, camera_entity, ecs_id(CameraSetting), sizeof(CameraSetting), &setting);
	ecs_set_id(world, camera_entity, ecs_id(LocalTransfrom), sizeof(LocalTransfrom), &trans);
	ecs_set_id(world, camera_entity, ecs_id(CameraViewProj), sizeof(CameraViewProj), &viewProjC);

	return camera_entity;
}

ecs_entity_t init_camera(ecs_world_t* world)
{
	ECS_COMPONENT(world, LocalTransfrom);

	CameraSetting camera_setting;
	camera_setting.orthoSize = 8.0f;
	camera_setting.farPlane = 100.0f;
	camera_setting.nearPlane = 0.1f;
	camera_setting.persMode = 0;
	camera_setting.fov = angle_to_radians(45);

	ecs_entity_t camera_entity = camera_create(world, camera_setting);
	LocalTransfrom* cam_pos = ecs_get_mut(world, camera_entity, LocalTransfrom);
	float3 pos = { 0.0f, 0, 5.0f };
	transform_set_position(cam_pos, pos);

	return camera_entity;
}