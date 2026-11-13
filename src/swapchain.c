#include "swapchain.h"
#include "helpers/log.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <vulkan/vulkan_core.h>


static VkSurfaceFormatKHR surfaceFormatSelect(VkSurfaceFormatKHR* formats, uint32_t numFormats);
static VkPresentModeKHR swapPresentModeSelect(VkPresentModeKHR* modes, uint32_t numModes);
static VkExtent2D extentSelect(VkSurfaceCapabilitiesKHR capabilities, GLFWwindow* window);
static uint32_t minImageCountSelect(VkSurfaceCapabilitiesKHR capabilities);


void createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, Swapchain* swapchain) {
	
	// ----------------------------------------
	// Aquire the capabilities of the surface
	// ----------------------------------------
	
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

	// -----------------------------------
	// Aquire the formats of the surface
	// -----------------------------------

	uint32_t surfaceFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, NULL);
	// FIXME: More bugchecks and logging for when this is in the hands of workshop people, we can't assume everyone will have a debugger setup
	if (surfaceFormatCount < 1) {
		log_fail("Inadequate number of Surface Format objects\n");
		return;
	}
	VkSurfaceFormatKHR* surfaceFormats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);

	// -----------------------------------------
	// Aquire the present modes of the surface
	// -----------------------------------------

	uint32_t surfacePresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, NULL);
	VkPresentModeKHR* surfacePresentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * surfacePresentModeCount);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, surfacePresentModes);

	// ------------------------------
	// Fill in the swapchain struct
	// ------------------------------

	swapchain->surfaceFormat = surfaceFormatSelect(surfaceFormats, surfaceFormatCount);
	swapchain->extent = extentSelect(surfaceCapabilities, window);

	// -------------------------------
	// Actually create the swapchain
	// -------------------------------

	VkSwapchainCreateInfoKHR createInfo = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.flags = 0, // Nothing ever happens
		.surface = surface,
		.minImageCount = minImageCountSelect(surfaceCapabilities),
		.imageFormat = swapchain->surfaceFormat.format,
		.imageColorSpace = swapchain->surfaceFormat.colorSpace,
		.imageExtent = swapchain->extent,
		.imageArrayLayers = 1, // If we wanted stereo video this would be higher
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0, // Not sharing, so this doesnt matter
		.pQueueFamilyIndices = NULL, // Still not sharing, we are greedy here
		.preTransform = surfaceCapabilities.currentTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.presentMode = swapPresentModeSelect(surfacePresentModes, surfacePresentModeCount),
		.clipped = VK_TRUE,
		.oldSwapchain = NULL, // Not an old swapchain on the first create
	};
	vkCreateSwapchainKHR(logicalDevice, &createInfo, NULL, &(swapchain->handle));

	// --------------------------
	// Get the swapchain images
	// --------------------------

	vkGetSwapchainImagesKHR(logicalDevice, swapchain->handle, &(swapchain->images.num), NULL);
	swapchain->images.imageHandles = (VkImage*)malloc(sizeof(VkImage) * swapchain->images.num);
	vkGetSwapchainImagesKHR(logicalDevice, swapchain->handle, &(swapchain->images.num), swapchain->images.imageHandles);

	// -----------------------
	// Free up old resources
	// -----------------------

	free(surfaceFormats); surfaceFormats = NULL;
	free(surfacePresentModes); surfacePresentModes = NULL;

	log_ok("Initial swapchain has been created\n");
}


void createImageViews(VkDevice device, Swapchain* swapchain) {
	if (!swapchain->handle) { log_warn("Skipping creating image views due to lack of swapchain\n"); return;}
	
	// --------------------------------------
	// Spec out all image views identically
	// --------------------------------------

	VkImageViewCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.format = swapchain->surfaceFormat.format,
		.subresourceRange = (VkImageSubresourceRange){
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, 
			.levelCount = 1, 
			.layerCount = 1, 
			.baseArrayLayer = 0, 
			.baseMipLevel = 0
		},
		.components = { // Make sure were just leaving color channels as they should be, but many a funny available here
			.r = VK_COMPONENT_SWIZZLE_IDENTITY, 
			.g = VK_COMPONENT_SWIZZLE_IDENTITY, 
			.b = VK_COMPONENT_SWIZZLE_IDENTITY, 
			.a = VK_COMPONENT_SWIZZLE_IDENTITY,
		},
		.viewType = VK_IMAGE_VIEW_TYPE_2D
	};

	// ----------------------
	// Allocate the handles
	// ----------------------

	swapchain->images.viewHandles = (VkImageView*)malloc(sizeof(VkImageView) * swapchain->images.num);
	assert(swapchain->images.imageHandles != NULL);
	
	// --------------------------------------
	// Actually create them "on" the device
	// --------------------------------------

	for (uint32_t i = 0; i < swapchain->images.num; i++) {
		createInfo.image = swapchain->images.imageHandles[i];
		vkCreateImageView(device, &createInfo, NULL, &(swapchain->images.viewHandles[i]));
	}

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
	assert(width > -1 && height > -1); // Detonate before the casting does
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
