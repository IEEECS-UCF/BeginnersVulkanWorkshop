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

// NOTE: Maybe move this and similar data structures into a different header file
typedef struct {
	uint32_t num;
	VkImage* imageHandles;
	VkImageView* viewHandles;
} ImageArrayList;

typedef struct {
	VkSwapchainKHR handle;
	ImageArrayList images;
	VkSurfaceFormatKHR surfaceFormat;
	VkExtent2D extent;
} Swapchain;
#define EMPTY_SWAPCHAIN_OBJECT (Swapchain){NULL, {0, NULL, NULL}, {0, 0}, {0,0}}


SwapchainResult createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, Swapchain* swapchain);

SwapchainResult createImageViews(VkDevice device, Swapchain* swapchain);

#endif
