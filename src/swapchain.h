#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "GLFW/glfw3.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

typedef enum {
	SWAPCHAIN_SUCCESS, // Swapchain and associated data has been successfully created and can proceed with execution safely
	SWAPCHAIN_MISSING_CAPABILITIES,
	SWAPCHAIN_NOT_ENOUGH_SURFACE_FORMATS,
} SwapchainResult;

typedef struct {
	VkSwapchainKHR handle;
	VkImage* images;
	uint32_t numImages;
	VkSurfaceFormatKHR surfaceFormat;
	VkExtent2D extent;
} Swapchain;

SwapchainResult createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, Swapchain* swapchain);

#endif
