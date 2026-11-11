#ifndef PHYSICALDEVICE_H
#define PHYSICALDEVICE_H

#include <vulkan/vulkan_core.h>

typedef enum {
	PHYS_NO_SUITABLE_DEVICE,
	PHYS_SUCCESS,
} PhysicalResult;

PhysicalResult selectPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice);

#endif
