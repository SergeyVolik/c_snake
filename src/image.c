
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "image.h"

void read_png_file(char* file_path)
{
	FILE* fp = fopen(file_path, "rb");

	char header[8];

	if (!fp) {

		log_error("[read_png_file] File %s could not be opened for reading", file_path);
		fprintf(stderr, "[read_png_file] File %s could not be opened for reading", file_path);
		return NULL;
	}

	fread(header, 1, 8, fp);

	int result = png_sig_cmp(header);
	log_info("is png %i", result);

	fclose(fp);
}

int png_sig_cmp(char* header)
{
	const char png_first_bytes[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

	for (size_t i = 0; i < 8; i++)
	{
		if (png_first_bytes[i] != header[i])
		{
			return 0;
		}
	}

	return 1;
}