#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "vulkan/vulkan_core.h"
#include <stdint.h>

typedef enum {
	COMMAND_POOL_SUCCESS,
	COMMAND_POOL_INPUTS_UNINITIALIZED,
	COMMAND_POOL_FAILURE,

	COMMAND_BUFFER_SUCCESS,
	COMMAND_BUFFER_ALLOC_FAIL,
	COMMAND_BUFFER_INPUTS_UNINITIALIZED,
} CommandBufResult;

CommandBufResult createCommandPool(VkDevice device, uint32_t queueIndex, VkCommandPool* commandPool);

CommandBufResult createCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBuffer* commandBuffer);

#endif
