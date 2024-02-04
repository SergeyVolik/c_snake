#include "LocalTransform.h"
#include <stdio.h>

LocalTransfrom transform_default()
{
	LocalTransfrom tran;

	Vec3 pos = { 0,0,0 };
	tran.position = pos;

	tran.scale = 1.0f;
	tran.rotation = 0;
	return tran;
}

void printf_mat4x4(mat4x4 mat)
{
	printf("matrix:\n");
	for (size_t i = 0; i < 4; i++)
	{
		printf("[ ");
		for (size_t j = 0; j < 4; j++)
		{
			printf("%f ", mat[i][j]);

			if (j != 3)
			{
				printf(",");
			}
		}
		printf("]\n");
	}
}

void transform_set_position(LocalTransfrom* trans, Vec3 pos)
{
	trans->position = pos;
}

Vec3 transform_get_position(LocalTransfrom* trans)
{
	return trans->position;
}