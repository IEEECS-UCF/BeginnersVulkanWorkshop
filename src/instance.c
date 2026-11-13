#include "instance.h"
#include "GLFW/glfw3.h"
#include "helpers/log.h"
#include "helpers/vulkanDebug.h"
#include "helpers/searching.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


static VkResult validateExtensions(uint32_t count, const char* const* extensions);
static VkResult validateLayers(uint32_t count, const char** layers);


void createInstance(VkInstance* instance) {

	// -----------------------
	// Spec application info
	// -----------------------

	


	// ----------------------------------------------------
	// Describe, then check, required instance extensions
	// ----------------------------------------------------

	

	// ----------------------------------------------------
	// Describe, then check, required instance layers
	// ----------------------------------------------------

	

	// ---------------------------------------
	// Debug messenger for instance creation
	// ---------------------------------------

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = instanceCreationDebugHook();

	// ---------------------
	// Create the instance
	// ---------------------

	

	// -------------------------
	// Free up unused resouces
	// -------------------------

	
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
