#ifndef LOCAL_TRANSFORM_H
#define LOCAL_TRANSFORM_H

#include "linmath.h"

typedef struct Vec2
{
	float x;
	float y;

} Vec2;

typedef struct Vec3
{
	float x;
	float y;
	float z;

} Vec3;

typedef struct LocalTransfrom
{
	Vec3 position;
	float rotation;
	float scale;

} LocalTransfrom;

typedef struct LocalToWorld
{
	mat4x4 value;

} LocalToWorld;

void transform_set_position(LocalTransfrom* trans, Vec3 pos);
Vec3 transform_get_position(LocalTransfrom* trans);

LocalTransfrom transform_default();
void printf_mat4x4(mat4x4 mat);

#endif
