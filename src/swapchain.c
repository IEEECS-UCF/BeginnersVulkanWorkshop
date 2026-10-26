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

SwapchainResult createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, Swapchain* swapchain) {
	VkResult res = VK_SUCCESS;
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
	if (res != VK_SUCCESS) {
		log_fail("Unable to get surface capabilities\n");
		return SWAPCHAIN_MISSING_CAPABILITIES;
	}

	uint32_t surfaceFormatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, NULL);
	// FIXME: More bugchecks and logging for when this is in the hands of workshop people, we can't assume everyone will have a debugger setup
	if (surfaceFormatCount < 1) {
		log_fail("Inadequate number of Surface Format objects\n");
		return SWAPCHAIN_NOT_ENOUGH_SURFACE_FORMATS;
	}
	VkSurfaceFormatKHR* surfaceFormats = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * surfaceFormatCount);
	assert(surfaceFormats != NULL);
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats);

	uint32_t surfacePresentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, NULL);
	assert(surfacePresentModeCount > 0);
	VkPresentModeKHR* surfacePresentModes = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * surfacePresentModeCount);
	assert(surfacePresentModes != NULL);
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &surfacePresentModeCount, surfacePresentModes);


	swapchain->surfaceFormat = surfaceFormatSelect(surfaceFormats, surfaceFormatCount);
	swapchain->extent = extentSelect(surfaceCapabilities, window);


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
	vkGetSwapchainImagesKHR(logicalDevice, swapchain->handle, &(swapchain->numImages), NULL);
	swapchain->images = (VkImage*)malloc(sizeof(VkImage) * swapchain->numImages);
	assert(swapchain->images != NULL);
	vkGetSwapchainImagesKHR(logicalDevice, swapchain->handle, &(swapchain->numImages), swapchain->images);

	free(surfaceFormats); surfaceFormats = NULL;
	free(surfacePresentModes); surfacePresentModes = NULL;

	log_ok("Initial swapchain has been created\n");

	return SWAPCHAIN_SUCCESS;
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
	uint32_t minImageCount = 3u > capabilities.minImageCount ? 3u : capabilities.minImageCount;
	// Makes sure that the max isn't exceeded, if so set it to the max (accounts for an unbound max)
	if ((0 < capabilities.maxImageCount) && (capabilities.maxImageCount < minImageCount)) {
		minImageCount = capabilities.maxImageCount;
	}

	return minImageCount;
}
