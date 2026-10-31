#include "commandBuffer.h"
#include "helpers/log.h"
#include <vulkan/vulkan_core.h>

CommandBufResult createCommandPool(VkDevice device, uint32_t queueIndex, VkCommandPool* commandPool) {
	if (!device) {
		log_warn("Command pool creator cannot detect a logical device\n");
		return COMMAND_POOL_INPUTS_UNINITIALIZED;
	}

	const VkCommandPoolCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // Allows for a new command buffer recording every frame
		.queueFamilyIndex = queueIndex
	};

	const VkResult res = vkCreateCommandPool(device, &createInfo, NULL, commandPool);
	if (res != VK_SUCCESS) {
		*commandPool = NULL;
		log_fail("Unable to create command pool for index %d\n", queueIndex);
		return COMMAND_POOL_FAILURE;
	}

	log_ok("Command pool has been created\n");
	return COMMAND_POOL_SUCCESS;
}

CommandBufResult createCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBuffer *commandBuffer) {
	if (!commandBuffer) {
		log_warn("Command buffer creator cannot detect a command pool\n");
		return COMMAND_BUFFER_INPUTS_UNINITIALIZED;
	}

	const VkCommandBufferAllocateInfo allocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	const VkResult res = vkAllocateCommandBuffers(device, &allocateInfo, commandBuffer);
	if (res != VK_SUCCESS) {
		*commandBuffer = NULL;
		log_fail("Command buffer allocation failed\n");
		return COMMAND_BUFFER_ALLOC_FAIL;
	}

	log_ok("Command buffer has been allocated\n");
	return COMMAND_BUFFER_SUCCESS;
}
