#include "fileio.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_platform.h>
#include "helpers/log.h"

uint32_t* readShaderBytecode(const char* filename, size_t* size) {
	FILE* fptr = fopen(filename, "rb");
	if (!fptr) {
		log_fail("Unable to open shader %s from disk\n", filename);
		return NULL;
	}

	fseek(fptr, 0, SEEK_END);
	long len = ftell(fptr);
	assert(len > 0);
	*size = (size_t)len;
	rewind(fptr);

	uint32_t* output = (uint32_t*)aligned_alloc(sizeof(uint32_t), (size_t)len+1);
	memset(output, 0, (size_t)len);
	assert(output);

	fread(output, (size_t)len, 1, fptr);
	fclose(fptr);

	return output;
}
