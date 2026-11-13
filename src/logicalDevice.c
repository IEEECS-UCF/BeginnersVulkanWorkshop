#include "logicalDevice.h"
#include "helpers/log.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


VkBool32 checkSurfaceSupport(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface);


void createLogicalDevice(VkPhysicalDevice physicalDevice, VkDevice* logicalDevice, VkSurfaceKHR surface, VkQueue* graphicsQueue, uint32_t* graphicsQueueIndex) {
	if (!physicalDevice) { log_warn("Skipping logical device creation due to lack of physical device\n"); return; }

	// -----------------------------
	// Get queue family properties
	// -----------------------------

	

	// ----------------------------------------------
	// Select a queue that matches our requirements
	// ----------------------------------------------

	

	// --------------------------------------------------
	// Queue create info needed after device is created
	// --------------------------------------------------

	

	// -----------------------------------------------
	// Opt into further features to ease development
	// -----------------------------------------------

	

	// --------------------------------
	// Final create info and creation
	// --------------------------------

	

	// ------------------
	// Get device queue
	// ------------------

	

	// --------------------------
	// Free up unused resources
	// --------------------------

	
	
	log_ok("Logical device has been created\n");
}

VkBool32 checkSurfaceSupport(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface) {
	VkBool32 support = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &support);
	return support;
}
