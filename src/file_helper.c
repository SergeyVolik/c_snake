
#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include "file_helper.h"
#include "log.h"

char* file_read_all_text(char* path)
{
	FILE* fp;
	long lSize;
	char* buffer;
	log_info("read_file");
	fp = fopen(path, "rb");
	if (!fp)
	{
		log_error("file cant be open. file:  %s", path);
		perror(path);
		exit(1);
	}

	fseek(fp, 0L, SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	log_info(" allocate memory for entire content");
	/* allocate memory for entire content */
	buffer = calloc(1, lSize + 1);

	if (!buffer)
	{
		log_error("memory alloc fails. file: %s", path);
		fclose(fp);
		fputs("memory alloc fails", stderr);
		exit(1);
	}
	/* copy the file into the buffer */
	if (1 != fread(buffer, lSize, 1, fp))
	{
		fclose(fp);
		free(buffer);
		log_error("entire read fails file: %s", path);
		fputs("entire read fails", stderr);
		exit(1);
	}

	/* do your work here, buffer is a string contains the whole text */
	log_info("content: %s", buffer);
	log_info("close file");
	fclose(fp);

	return buffer;
}