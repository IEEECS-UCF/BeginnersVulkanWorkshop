#include "logicalDevice.h"
#include "helpers/log.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


VkBool32 checkSurfaceSupport(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface);


void createLogicalDevice(VkPhysicalDevice physicalDevice, VkDevice* logicalDevice, VkSurfaceKHR surface, VkQueue* graphicsQueue, uint32_t* graphicsQueueIndex) {
	if (!physicalDevice) { log_warn("Skipping logical device creation due to lack of physical device\n"); return; }

	// -----------------------------
	// Get queue family properties
	// -----------------------------

	/// Getting the queue create info ready to feed into the main logical device creation
	// Fetching queue family info for selection of those that meet our need
	uint32_t numQueueFamilyProperties = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilyProperties, NULL);
	VkQueueFamilyProperties* queueFamilyProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * numQueueFamilyProperties);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilyProperties, queueFamilyProperties);

	// ----------------------------------------------
	// Select a queue that matches our requirements
	// ----------------------------------------------

	// Just the first one we can find, guaranteed to be there by physical device creation
	uint32_t graphicsIndex = 0;
	for (uint32_t i = 0; i < numQueueFamilyProperties; i++) {
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
			checkSurfaceSupport(physicalDevice, i, surface)
		) {
			graphicsIndex = i;
			break;
		}
	}

	// --------------------------------------------------
	// Queue create info needed after device is created
	// --------------------------------------------------

	// Collective queue metadata for retrieval later
	float prority = 0.5f;
	VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
		.queueFamilyIndex = graphicsIndex,
		.queueCount = 1,
		.pQueuePriorities = &prority, // Expects an array, so pass one as a pointer
	};
	*graphicsQueueIndex = graphicsIndex;

	// -----------------------------------------------
	// Opt into further features to ease development
	// -----------------------------------------------

	/// Opting into specific features of the physical device so they are available according to the logical device
	// More easier things
	VkPhysicalDeviceExtendedDynamicStateFeaturesEXT deviceDynamicFeatures;
	deviceDynamicFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
	deviceDynamicFeatures.extendedDynamicState = VK_TRUE;
	deviceDynamicFeatures.pNext = NULL;
	// Makes writing command queues and pipelines infinitely easier (less rewritten code)
	VkPhysicalDeviceVulkan13Features device13Features;
	memset(&device13Features, VK_FALSE, sizeof(VkPhysicalDeviceVulkan13Features));
	device13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	device13Features.dynamicRendering = VK_TRUE;
	device13Features.synchronization2 = VK_TRUE;
	device13Features.pNext = &deviceDynamicFeatures;
	// Eases shader writing
	VkPhysicalDeviceVulkan11Features device11Features;
	memset(&device11Features, 0, sizeof(VkPhysicalDeviceVulkan11Features));
	device11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	device11Features.shaderDrawParameters = VK_TRUE;
	device11Features.pNext = &device13Features;
	// Carrier structure for the full chain (in our use case)
	VkPhysicalDeviceFeatures2 deviceFeatures;
	deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceFeatures.pNext = &device11Features;
	memset(&deviceFeatures.features, VK_FALSE, sizeof(VkPhysicalDeviceFeatures));

	// --------------------------------
	// Final create info and creation
	// --------------------------------

	/// Extensions that we need for rendering
	const char* requiredExtensions[1] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	/// Actual data and call to create the device
	VkDeviceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.flags = 0,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = requiredExtensions,
		.pEnabledFeatures = NULL, // If it were the normal datastructure 'VkPhysicalDeviceFeatures' it would go here
		.pNext = &deviceFeatures, // Supports our extended chain of features
	};
	vkCreateDevice(physicalDevice, &createInfo, NULL, logicalDevice);

	// ------------------
	// Get device queue
	// ------------------

	// Queue that we will dispatch all calls onto later on
	vkGetDeviceQueue(*logicalDevice, graphicsIndex, 0, graphicsQueue);

	// --------------------------
	// Free up unused resources
	// --------------------------

	free(queueFamilyProperties);
	log_ok("Logical device has been created\n");
}

VkBool32 checkSurfaceSupport(VkPhysicalDevice physicalDevice, uint32_t queueFamilyIndex, VkSurfaceKHR surface) {
	VkBool32 support = VK_FALSE;
	vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, surface, &support);
	return support;
}
