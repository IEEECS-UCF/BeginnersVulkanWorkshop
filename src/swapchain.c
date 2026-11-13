#include "swapchain.h"
#include "helpers/log.h"
#include <stdint.h>
#include <stdlib.h>


static VkSurfaceFormatKHR surfaceFormatSelect(VkSurfaceFormatKHR* formats, uint32_t numFormats);
static VkPresentModeKHR swapPresentModeSelect(VkPresentModeKHR* modes, uint32_t numModes);
static VkExtent2D extentSelect(VkSurfaceCapabilitiesKHR capabilities, GLFWwindow* window);
static uint32_t minImageCountSelect(VkSurfaceCapabilitiesKHR capabilities);


void createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, Swapchain* swapchain) {
	if (!logicalDevice) { log_warn("Skipping creating swapchain due to lack of needed inputs\n"); return;}
	
	// ----------------------------------------
	// Aquire the capabilities of the surface
	// ----------------------------------------
	
	

	// -----------------------------------
	// Aquire the formats of the surface
	// -----------------------------------

	

	// -----------------------------------------
	// Aquire the present modes of the surface
	// -----------------------------------------

	

	// ------------------------------
	// Fill in the swapchain struct
	// ------------------------------

	

	// -------------------------------
	// Actually create the swapchain
	// -------------------------------

	

	// --------------------------
	// Get the swapchain images
	// --------------------------

	

	// -----------------------
	// Free up old resources
	// -----------------------

	

	log_ok("Initial swapchain has been created\n");
}


void createImageViews(VkDevice device, Swapchain* swapchain) {
	if (!swapchain->handle) { log_warn("Skipping creating image views due to lack of swapchain\n"); return;}
	
	// --------------------------------------
	// Spec out all image views identically
	// --------------------------------------

	

	// ----------------------
	// Allocate the handles
	// ----------------------

	
	
	// --------------------------------------
	// Actually create them "on" the device
	// --------------------------------------

	

	log_ok("Swapchain image views have been created\n");
}


// Seeks out a SRGB [BGRA ordering] color space, if not present returns the first format available
static VkSurfaceFormatKHR surfaceFormatSelect(VkSurfaceFormatKHR* formats, uint32_t numFormats) {
	for (uint32_t i = 0; i < numFormats; i++) {
		if (formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && formats[i].format == VK_FORMAT_B8G8R8A8_SRGB)
			return formats[i];
	}

	return formats[0]; // Guaranteed by prior assertions, would be better to do a search for a "better" format (that is supported)
}

//
static VkPresentModeKHR swapPresentModeSelect(VkPresentModeKHR* modes, uint32_t numModes) {
	// Mailbox is a variation on FIFO where if the queue is full, the program overwrites old data instead of waiting
	// In practice acting as a "triple buffered" system, also avoids tearing issues (basically better VSYNC)
	for (uint32_t i = 0; i < numModes; i++) {
		if (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return VK_PRESENT_MODE_MAILBOX_KHR;
	}

	return VK_PRESENT_MODE_FIFO_KHR; // There's a swapchain, there will be FIFO
}

// Bound an arbitray uint32_t between two values
static uint32_t clamp(uint32_t value, uint32_t min, uint32_t max) {
	const uint32_t min_compensated = value < min ? min : value;
	return min_compensated > max ? max : min_compensated;
}

//
static VkExtent2D extentSelect(VkSurfaceCapabilitiesKHR capabilities, GLFWwindow* window) {
	if (capabilities.currentExtent.width != UINT32_MAX) // Window manager isn't doing some funny business like HiDPI, so we just spit back the res
		return capabilities.currentExtent;
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	return (VkExtent2D){
		.width = clamp((uint32_t)width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
		.height = clamp((uint32_t)height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
	};
}

//
static uint32_t minImageCountSelect(VkSurfaceCapabilitiesKHR capabilities) {
	// Asks for at least 3 images
	uint32_t minImageCount = 2u > capabilities.minImageCount ? 2u : capabilities.minImageCount;
	// Makes sure that the max isn't exceeded, if so set it to the max (accounts for an unbound max)
	if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount)) {
		minImageCount = capabilities.maxImageCount;
	}

	return minImageCount;
}
