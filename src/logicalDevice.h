#ifndef LOGICALDEVICE_H
#define LOGICALDEVICE_H

#include <stdint.h>
#include <vulkan/vulkan_core.h>

void createLogicalDevice(VkPhysicalDevice physicalDevice, VkDevice* logicalDevice, VkSurfaceKHR surface, VkQueue* graphicsQueue, uint32_t* graphicsQueueIndex);

#endif
