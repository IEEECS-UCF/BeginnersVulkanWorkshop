#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "GLFW/glfw3.h"
#include <stdint.h>
#include <vulkan/vulkan_core.h>

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


void createSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, Swapchain* swapchain);

void createImageViews(VkDevice device, Swapchain* swapchain);

#endif
