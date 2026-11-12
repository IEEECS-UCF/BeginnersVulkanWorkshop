#include "draw.h"
#include "commandBuffer.h"
#include "helpers/log.h"
#include <assert.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>


void createSynchronizationObjects(VkDevice device, SyncObjects *objects, uint32_t numImages) {
	if (device == NULL) { log_warn("Skipping creating sync objects as inputs uninitialized\n"); return; }

	// -----------------------------------------
	// Presentation synchronization semaphores
	// -----------------------------------------

	VkSemaphoreCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
	};
	vkCreateSemaphore(device, &createInfo, NULL, &(objects->presentComplete));

	// ----------------------------------
	// Image synchronization semaphores
	// ----------------------------------

	objects->renderComplete = malloc(sizeof(VkSemaphore) * numImages);
	for (uint32_t i = 0; i < numImages; i++) {
		vkCreateSemaphore(device, &createInfo, NULL, &(objects->renderComplete[i]));
	}

	// -------------------------------------------------
	// Drawing process fence to prevent duplicate work
	// -------------------------------------------------

	// The underlying datastructure has an identical memory footprint, so why waste resources
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // So we dont block on first draw
	vkCreateFence(device, (VkFenceCreateInfo*)&createInfo, NULL, &(objects->drawFence));
}

void drawFrame(VkDevice device, SyncObjects sync, Swapchain swapchain, VkCommandBuffer commandBuffer, VkQueue graphicsQueue , VkPipeline graphicsPipeline) {
	if (commandBuffer == NULL || sync.drawFence == NULL) { log_warn("Skipping drawing as inputs uninitialized"); return; }

	// --------------------
	// Fence draw blocker
	// --------------------

	vkWaitForFences(device, 1, &sync.drawFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &sync.drawFence);

	// ---------------------------------------------
	// Get the next image index from the swapchain
	// ---------------------------------------------

	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(device, swapchain.handle, UINT64_MAX, sync.presentComplete, NULL, &imageIndex);

	// ----------------------------
	// Record in a command buffer
	// ----------------------------

	vkResetCommandBuffer(commandBuffer, 0);
	recordCommandBuffer(commandBuffer, swapchain, imageIndex, graphicsPipeline);

	// ------------------------------------
	// Submitting command buffer to queue
	// ------------------------------------

	VkPipelineStageFlags stageFlags[1] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	VkSemaphore waitSemaphores[1] = {sync.presentComplete};
	VkSemaphore signalSemaphore[1] = {sync.renderComplete[imageIndex]};
	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphores,
		.pWaitDstStageMask = stageFlags,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphore
	};
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, sync.drawFence);

	// ------------------------------------------------------------
	// Line up the image for presentation in an appropriate queue
	// ------------------------------------------------------------

	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &swapchain.handle,
		.pImageIndices = &imageIndex,
		NULL // Only one swapchain so itll be obvious when it fails
	};
	vkQueuePresentKHR(graphicsQueue, &presentInfo);
}
