#include "draw.h"
#include "commandBuffer.h"
#include "helpers/log.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan_core.h>


void createSynchronizationObjects(VkDevice device, SyncObjects *objects, uint32_t numImages) {
	if (device == NULL) { log_warn("Skipping creating sync objects as inputs uninitialized\n"); return; }

	// -----------------------------------------
	// Presentation synchronization semaphores
	// -----------------------------------------

	

	// ----------------------------------
	// Image synchronization semaphores
	// ----------------------------------

	

	// -------------------------------------------------
	// Drawing process fence to prevent duplicate work
	// -------------------------------------------------


}


void drawFrame(VkDevice device, SyncObjects sync, Swapchain swapchain, VkCommandBuffer commandBuffer, VkQueue graphicsQueue , VkPipeline graphicsPipeline) {
	if (commandBuffer == NULL || sync.drawFence == NULL) { log_warn("Skipping drawing as inputs uninitialized\n"); return; }

	// --------------------
	// Fence draw blocker
	// --------------------

	

	// ---------------------------------------------
	// Get the next image index from the swapchain
	// ---------------------------------------------

	

	// ----------------------------
	// Record in a command buffer
	// ----------------------------

	

	// ------------------------------------
	// Submitting command buffer to queue
	// ------------------------------------

	

	// ------------------------------------------------------------
	// Line up the image for presentation in an appropriate queue
	// ------------------------------------------------------------

	
}
