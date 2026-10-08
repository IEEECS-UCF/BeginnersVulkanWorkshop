#ifndef LOGICALDEVICE_H
#define LOGICALDEVICE_H

#include <vulkan/vulkan_core.h>

VkResult createLogicalDevice(VkPhysicalDevice physicalDevice, VkDevice* logicalDevice, VkQueue* graphicsQueue);

#endif
