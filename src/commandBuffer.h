#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include "swapchain.h"
#include "vulkan/vulkan_core.h"
#include <stdint.h>

void createCommandPool(VkDevice device, uint32_t queueIndex, VkCommandPool* commandPool);

void createCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBuffer* commandBuffer);

void recordCommandBuffer(VkCommandBuffer commandBuffer, Swapchain swapchain, uint32_t imageIndex, VkPipeline graphicsPipeline);

#endif
