#ifndef LOCAL_TRANSFORM_H
#define LOCAL_TRANSFORM_H

#include "linmath.h"

typedef struct float2
{
	float x;
	float y;

} float2;

typedef struct float3
{
	float x;
	float y;
	float z;

} float3;

typedef struct float4
{
	float x;
	float y;
	float z;
	float w;
} float4;

inline static float3 float3_up()
{
	float3 res = {0};
	res.y = 1;
	return res;
}

inline static float3 float3_right()
{
	float3 res = { 0 };
	res.x = 1;
	return res;
}

inline static float3 float3_forward()
{
	float3 res = { 0 };
	res.z = 1;
	return res;
}

inline static float2 float2_up()
{
	float2 res = { 0 };
	res.y = 1;
	return res;
}

inline static float2 float2_right()
{
	float2 res = { 0 };
	res.x = 1;
	return res;
}

inline static float3 f2_to_f3(float2 a)
{
	float3 res;
	res.x = a.x;
	res.y = a.y;
	return res;
}

inline static float3 float3_add(float3 a, float3 b)
{
	float3 res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;

	return res;
}

inline static float3 float3_div(float3 a, float div)
{
	float3 res;
	res.x = a.x / div;
	res.y = a.y / div;
	res.z = a.z / div;

	return res;
}

inline static float3 float3_mul(float3 a, float mul)
{
	float3 res;
	res.x = a.x * mul;
	res.y = a.y * mul;
	res.z = a.z * mul;

	return res;
}

inline static float3 float3_sub(float3 a, float3 b)
{
	float3 res;
	res.x = a.x - b.x;
	res.y = a.y - b.y;
	res.z = a.z - b.z;

	return res;
}

inline static float2 float2_sub(float2 a, float2 b)
{
	float2 res;
	res.x = a.x - b.x;
	res.y = a.y - b.y;
	return res;
}

inline static float2 float2_add(float2 a, float2 b)
{
	float2 res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	return res;
}

inline static float2 float2_mul(float2 a, float b)
{
	float2 res;
	res.x = a.x * b;
	res.y = a.y * b;
	return res;
}

inline static float2 float2_div(float2 a, float b)
{
	float2 res;
	res.x = a.x / b;
	res.y = a.y / b;
	return res;
}

typedef struct LocalTransfrom
{
	float3 position;
	float rotation;
	float scale;

} LocalTransfrom;

typedef struct LocalToWorld
{
	mat4x4 value;

} LocalToWorld;

void transform_set_position(LocalTransfrom* trans, float3 pos);
float3 transform_get_position(LocalTransfrom* trans);

LocalTransfrom transform_default();
LocalTransfrom transform_pos(float3 pos);

void printf_mat4x4(mat4x4 mat);

#endif
