#include "commandBuffer.h"
#include "helpers/log.h"
#include "swapchain.h"


void createCommandPool(VkDevice device, uint32_t queueIndex, VkCommandPool* commandPool) {
	if (!device) { log_warn("Skipping creating command pool due to lack of logical device\n"); return; }

	// -----------------------------------------------
	// Create the command pool from the queue family
	// -----------------------------------------------

	

	log_ok("Command pool has been created\n");
}


void createCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBuffer *commandBuffer) {
	if (!commandPool) { log_warn("Skipping creating command buffer due to lack of command pool\n"); return; }

	// -------------------------------------------------
	// Allocate the command buffer on the command pool
	// -------------------------------------------------

	

	log_ok("Command buffer has been allocated\n");
}


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


void recordCommandBuffer(VkCommandBuffer commandBuffer, Swapchain swapchain, uint32_t imageIndex, VkPipeline graphicsPipeline) {

	// -------------------------------------------
	// Indicate beginning of full command buffer
	// -------------------------------------------

	

	// -------------------------------------
	// Prepare how image should be treated
	// -------------------------------------

	

	// --------------------------------------------------------------------------
	// Specify how this run of rendering should behave with repect to the image
	// --------------------------------------------------------------------------

	

	// --------------------------------------------
	// Startup the rendering system on a pipeline
	// --------------------------------------------

	


	// --------------------------------------------
	// Setup dynamic state (viewport and scissor)
	// --------------------------------------------

	// ----- Viewport -----

	

	// ----- Scissor -----

	

	// ----------
	// Draw it!
	// ----------

	

	// -------------------------------
	// Close out this command buffer
	// -------------------------------

	
}
