#include "draw.h"
#include "commandBuffer.h"
#include "helpers/log.h"
#include <assert.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>

void createSynchronizationObjects(VkDevice device, SyncObjects *objects) {
	VkSemaphoreCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
	};
	vkCreateSemaphore(device, &createInfo, NULL, &(objects->presentComplete));
	vkCreateSemaphore(device, &createInfo, NULL, &(objects->renderComplete));


	// The underlying datastructure has an identical memory footprint, so why waste resources
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // So we dont block on first draw
	vkCreateFence(device, (VkFenceCreateInfo*)&createInfo, NULL, &(objects->drawFence));
}

void drawFrame(VkDevice device, SyncObjects *sync, Swapchain swapchain, VkCommandBuffer commandBuffer, VkQueue graphicsQueue , VkPipeline graphicsPipeline) {
	// Locking mechanism for ensuring the prior frame is in step between GPU and CPU
	VkResult fenceRes = vkWaitForFences(device, 1, &(sync->drawFence), VK_TRUE, UINT64_MAX);
	assert(fenceRes == VK_SUCCESS); // Something really wrong has happened
	vkResetFences(device, 1, &(sync->drawFence));

	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(device, swapchain.handle, UINT64_MAX, sync->presentComplete, NULL, &imageIndex);

	vkResetCommandBuffer(commandBuffer, 0);
	recordCommandBuffer(commandBuffer, swapchain, imageIndex, graphicsPipeline);

	VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(sync->presentComplete),
		.pWaitDstStageMask = &stageFlags,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &(sync->renderComplete)
	};
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, sync->drawFence);

	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &(sync->renderComplete),
		.swapchainCount = 1,
		.pSwapchains = &swapchain.handle,
		.pImageIndices = &imageIndex,
		NULL // Only one swapchain so itll be obvious when it fails
	};
	vkQueuePresentKHR(graphicsQueue, &presentInfo);
}
