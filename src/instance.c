#include "instance.h"
#include "GLFW/glfw3.h"
#include "helpers/log.h"
#include "helpers/vulkanDebug.h"
#include "helpers/searching.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

static VkResult validateExtensions(uint32_t count, const char* const* extensions);
static VkResult validateLayers(uint32_t count, const char** layers);

void createInstance(VkInstance* instance) {
	// Describes the program in broad strokes to the driver for it to maybe do some optimizations
	const VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = "Vulkanization!",
		.applicationVersion = VK_MAKE_VERSION(0, 1, 0),
		.pEngineName = "No Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_4, // Latest API version, no reason to go back
	};


	// Extensions defined by us to need
	const char* additionalExtensions[1] = {
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
	};
	uint32_t extensionCount = 0;
	// GLFW will tell us what extensions are required for the instance to graphically function
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	uint32_t allocCount = extensionCount + (sizeof(additionalExtensions) / sizeof(char*));
	const char** extensions = (const char* *)malloc(sizeof(char*) * allocCount);
	assert(extensions != NULL);
	memcpy((void*)extensions, glfwExtensions, extensionCount * sizeof(char*)); // Move all glfw required extensions into the final array
	memcpy((void*)(extensions + extensionCount), additionalExtensions, sizeof(additionalExtensions));
	extensionCount = allocCount;
	VkResult resExtensions = validateExtensions(extensionCount, extensions);
 	if (resExtensions == VK_ERROR_EXTENSION_NOT_PRESENT) { // Only condition where a bailout is required
		free(extensions);
		return;
	}

	// Layers defined by us to need in execution
	const char* layers[1] = {
		"VK_LAYER_KHRONOS_validation",
	};
	uint32_t layerCount = sizeof(layers) / sizeof(char*);
	VkResult resLayers = validateLayers(layerCount, layers);
	if (resLayers == VK_ERROR_LAYER_NOT_PRESENT) {
		return;
	}


	// Keep the hook description in the stack for this sole event
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = instanceCreationDebugHook();
	// Drilled down list of requirements that will be pulled into the instance by the driver
	// and other helpers on the system
	const VkInstanceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = &debugCreateInfo,
		.flags = 0, // Only portability flag, so nothing here
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = layerCount,
		.ppEnabledLayerNames = layers,
		.enabledExtensionCount = extensionCount,
		.ppEnabledExtensionNames = extensions,
	};

	VkResult res = vkCreateInstance(&createInfo, NULL, instance);
	free(extensions);
	if (res != VK_SUCCESS) 
		log_fail("Couldn't create Vulkan instance");
	else
		log_ok("Instance has been created\n");
	return;
}


static VkResult validateExtensions(uint32_t count, const char* const* extensions) {
	VkResult res = VK_SUCCESS;
	uint32_t presentExtensionCount = 0;
	VkExtensionProperties* presentExtensions = NULL;

	// Output exactly how many extensions are present in the implemntation and implicit layers to check against
	res = vkEnumerateInstanceExtensionProperties(NULL, &presentExtensionCount, NULL);
	if (res != VK_SUCCESS) {
		log_warn("Inital extension enumeration failed, unable to validate\n");
		return res;
	}

	presentExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * presentExtensionCount);
	res = vkEnumerateInstanceExtensionProperties(NULL, &presentExtensionCount, presentExtensions);
	if (res != VK_SUCCESS) {
		log_warn("Secondary extension enumeration failed, unable to validate\n");
		return res;
	}


	ExtensionSearchResult searchRes = searchExtensions(presentExtensionCount, presentExtensions, count, extensions);
	if (searchRes == MISSING_EXTENSIONS) {
		log_fail("Instance missing required extensions!\n");
		free(presentExtensions);
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}


	free(presentExtensions);
	log_debg("All extensions validated\n");
	return res;
}

static VkResult validateLayers(uint32_t count, const char** layers) {
	VkResult res = VK_SUCCESS;
	uint32_t presentLayerCount = 0;
	VkLayerProperties* presentLayers = NULL;

	res = vkEnumerateInstanceLayerProperties(&presentLayerCount, NULL);
	if (res != VK_SUCCESS) {
		log_warn("Inital layer enumeration failed, unable to validate\n");
		return res;
	}

	presentLayers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * presentLayerCount);
	res = vkEnumerateInstanceLayerProperties(&presentLayerCount, presentLayers);
	if (res != VK_SUCCESS) {
		log_warn("Secondary layer enumeration failed, unable to validate\n");
		return res;
	}


	for (uint32_t i = 0; i < count; i++) {
		uint8_t found = 0;
		for (uint32_t j = 0; j < presentLayerCount; j++) {
			if (strcmp(presentLayers[j].layerName, layers[i]) == 0) {
				found++;
				// PERF: Could swap current and last to save a few runs, but eh
				break;
			}
		}
		if (found == 0) {
			free(presentLayers);
			log_fail("Couldn't validate layer \"%s\"\n", layers[i]);
			return VK_ERROR_LAYER_NOT_PRESENT;
		}
	}


	free(presentLayers);
	log_debg("All layers validated\n");
	return VK_SUCCESS;
}
