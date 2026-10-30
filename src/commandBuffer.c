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

	VkResult res = vkCreateCommandPool(device, &createInfo, NULL, commandPool);
	if (res != VK_SUCCESS) {
		*commandPool = NULL;
		log_fail("Unable to create command pool for index %d\n", queueIndex);
		return COMMAND_POOL_FAILURE;
	}

	log_ok("Command pool has been created\n");
	return COMMAND_POOL_SUCCESS;
}
