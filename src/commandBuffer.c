#include "commandBuffer.h"
#include "helpers/log.h"
#include "swapchain.h"
#include <vulkan/vulkan.h>
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


//
static void transitionImageLayout(
		VkCommandBuffer commandBuffer,
		uint32_t imageIndex,
		ImageArrayList swapchainImages,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		VkAccessFlags2 srcAccessMask,
		VkAccessFlags2 dstAccessMask,
		VkPipelineStageFlags2 srcStageMask,
		VkPipelineStageFlags2 dstStageMask
) {
	VkImageMemoryBarrier2 barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.pNext = NULL,
		.srcStageMask = srcStageMask,
		.dstStageMask = dstStageMask,
		.srcAccessMask = srcAccessMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = oldLayout,
		.newLayout = newLayout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapchainImages.imageHandles[imageIndex],
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};

	VkDependencyInfo dependencyInfo = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.pNext = NULL,
		.dependencyFlags = 0,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier,
		.bufferMemoryBarrierCount = 0,
		.pBufferMemoryBarriers = NULL,
		.memoryBarrierCount = 0,
		.pMemoryBarriers = NULL
	};

	vkCmdPipelineBarrier2(commandBuffer, &dependencyInfo);
}


// 
CommandBufResult recordCommandBuffer(VkCommandBuffer commandBuffer, Swapchain swapchain, uint32_t imageIndex, VkPipeline graphicsPipeline) {
	VkResult res = VK_SUCCESS;

	const VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	};
	res = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	if (res != VK_SUCCESS) {
		log_fail("Beginning command buffer recording failed\n");
		return COMMAND_BUFFER_BEGIN_FAIL;
	}


	transitionImageLayout(
			commandBuffer,
			imageIndex, 
			swapchain.images,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_ACCESS_2_NONE,
			VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
	);

	VkClearValue clearColor = { .color = { .float32 = {0.0f, 0.0f, 0.0f, 1.0f} } };
	VkRenderingAttachmentInfo attachmentInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.pNext = NULL,
		.imageView = swapchain.images.viewHandles[imageIndex],
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = clearColor
	};

	VkRenderingInfo renderingInfo = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.pNext = NULL,
		.flags = 0,
		.renderArea = { .offset = {0,0}, .extent = swapchain.extent},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentInfo,
		.pDepthAttachment = NULL,
		.pStencilAttachment = NULL
	};

	vkCmdBeginRendering(commandBuffer, &renderingInfo);


	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	VkViewport viewport = {
		.x = 0.0f, .y = 0.0f,
		.width = (float)swapchain.extent.width,
		.height = (float)swapchain.extent.height,
		.minDepth = 0.0f, .maxDepth = 1.0f
	};
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = { .offset = {0,0}, swapchain.extent};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);


	vkCmdEndRendering(commandBuffer);

	transitionImageLayout(
			commandBuffer,
			imageIndex,
			swapchain.images,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			VK_ACCESS_2_NONE,
			VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
	);

	res = vkEndCommandBuffer(commandBuffer);
	if (res != VK_SUCCESS) {
		log_fail("Ending of command buffer encountered error\n");
		return COMMAND_BUFFER_END_FAIL;
	}

	return COMMAND_BUFFER_SUCCESS;
}


