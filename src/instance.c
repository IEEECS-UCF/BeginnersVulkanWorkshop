#include "instance.h"
#include "helpers/vulkanDebug.h"
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

void createInstance(VkInstance* instance) {

	const VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = "Vulkanization!",
		.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_4, // Latest API version, no reason to go back
	};

	// Keep the hook description in the stack for this sole event
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = instanceCreationDebugHook();
	const VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = &debugCreateInfo,
		.flags = 0, // Only portability flag, so nothing here
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = 0,
		.ppEnabledLayerNames = NULL,
		.enabledExtensionCount = 0,
		.ppEnabledExtensionNames = NULL,
	};

	VkResult res = vkCreateInstance(&createInfo, NULL, instance);

	return;
}
