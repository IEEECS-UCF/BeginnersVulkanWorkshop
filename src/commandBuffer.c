#include "commandBuffer.h"
#include "helpers/log.h"
#include "swapchain.h"


void createCommandPool(VkDevice device, uint32_t queueIndex, VkCommandPool* commandPool) {
	if (!device) { log_warn("Skipping creating command pool due to lack of logical device\n"); return; }

	// -----------------------------------------------
	// Create the command pool from the queue family
	// -----------------------------------------------

	const VkCommandPoolCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, // Allows for a new command buffer recording every frame
		.queueFamilyIndex = queueIndex
	};
	vkCreateCommandPool(device, &createInfo, NULL, commandPool);

	log_ok("Command pool has been created\n");
}


void createCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBuffer *commandBuffer) {
	if (!commandPool) { log_warn("Skipping creating command buffer due to lack of command pool\n"); return; }

	// -------------------------------------------------
	// Allocate the command buffer on the command pool
	// -------------------------------------------------

	const VkCommandBufferAllocateInfo allocateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	vkAllocateCommandBuffers(device, &allocateInfo, commandBuffer);

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

	const VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = NULL
	};
	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	// -------------------------------------
	// Prepare how image should be treated
	// -------------------------------------

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

	// --------------------------------------------------------------------------
	// Specify how this run of rendering should behave with repect to the image
	// --------------------------------------------------------------------------

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

	// --------------------------------------------
	// Startup the rendering system on a pipeline
	// --------------------------------------------

	vkCmdBeginRendering(commandBuffer, &renderingInfo);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);


	// --------------------------------------------
	// Setup dynamic state (viewport and scissor)
	// --------------------------------------------

	// ----- Viewport -----

	VkViewport viewport = {
		.x = 0.0f, .y = 0.0f,
		.width = (float)swapchain.extent.width,
		.height = (float)swapchain.extent.height,
		.minDepth = 0.0f, .maxDepth = 1.0f
	};
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	// ----- Scissor -----

	VkRect2D scissor = { .offset = {0,0}, swapchain.extent};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// ----------
	// Draw it!
	// ----------

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	// -------------------------------
	// Close out this command buffer
	// -------------------------------

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

	vkEndCommandBuffer(commandBuffer);
}
