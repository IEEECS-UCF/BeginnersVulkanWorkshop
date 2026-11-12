#ifndef DRAW_H
#define DRAW_H

#include "swapchain.h"
#include <vulkan/vulkan_core.h>

typedef struct {
	VkSemaphore presentComplete;
	VkSemaphore* renderComplete;
	VkFence drawFence;
} SyncObjects;
#define EMPTY_SYNC_OBJECTS (SyncObjects){NULL, NULL, NULL}

void createSynchronizationObjects(VkDevice device, SyncObjects* objects, uint32_t numImages);

void drawFrame(VkDevice device, SyncObjects *sync, Swapchain swapchain, VkCommandBuffer commandBuffer, VkQueue graphicsQueue , VkPipeline graphicsPipeline);

#endif
