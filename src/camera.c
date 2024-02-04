#include "camera.h"
#include "LocalTransform.h"

ecs_entity_t camera_create(ecs_world_t* world, CameraSetting setting)
{
	ECS_COMPONENT(world, CameraSetting);
	ECS_COMPONENT(world, LocalTransfrom);
	ECS_COMPONENT(world, CameraViewProj);

	ECS_TAG(world, camera_tag);

	ecs_entity_t camera_entity = ecs_entity(world, { .name = "Camera" });
	LocalTransfrom trans = transform_default();
	CameraViewProj viewProjC = {0};

	ecs_add_id(world, camera_entity, camera_tag);
	ecs_set_id(world, camera_entity, ecs_id(CameraSetting), sizeof(CameraSetting), &setting);
	ecs_set_id(world, camera_entity, ecs_id(LocalTransfrom), sizeof(LocalTransfrom), &trans);
	//ecs_set_id(world, camera_entity, ecs_id(CameraViewProj), sizeof(CameraViewProj), &viewProjC);

	return camera_entity;
}

ecs_entity_t init_camera(ecs_world_t* world)
{
	ECS_COMPONENT(world, LocalTransfrom);

	CameraSetting camera_setting;
	camera_setting.orthoSize = 2.0f;
	camera_setting.farPlane = 100.0f;
	camera_setting.nearPlane = -0.01f;

	ecs_entity_t camera_entity = camera_create(world, camera_setting);
	LocalTransfrom* cam_pos = ecs_get_mut(world, camera_entity, LocalTransfrom);
	Vec3 pos = { 1.0f, 0, 0 };
	transform_set_position(cam_pos, pos);

	return camera_entity;
}