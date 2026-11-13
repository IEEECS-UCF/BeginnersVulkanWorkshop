#include "physicalDevice.h"
#include "helpers/log.h"
#include "helpers/searching.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#define SUITABLE 1
#define UNSUITABLE 0
#define TRUE 1
#define FALSE 0


static uint8_t validateDevice(VkPhysicalDevice device);


void selectPhysicalDevice(VkInstance instance, VkPhysicalDevice* physicalDevice) {
	if (!instance) { log_warn("\n"); return; }

	// ----------------------
	// Get physical devices
	// ----------------------
	
	uint32_t numDevices = 0;
	VkPhysicalDevice* devices = NULL;
	vkEnumeratePhysicalDevices(instance, &numDevices, NULL);
	devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * numDevices);
	vkEnumeratePhysicalDevices(instance, &numDevices, devices);

	// ----------------------------------------------------------
	// Check each device and pick the best (first valid) option
	// ----------------------------------------------------------

	for (uint32_t i = 0; i < numDevices; i++) {
		// Ensure device has the minimum requirements, skip if it fails
		if (!validateDevice(devices[i]))
			continue;

		// INFO: Some further ranking of devices and such could and should be done here, but isn't for the sake of brevity
		
		*physicalDevice = devices[i];
	}

	// ----------------------------------
	// Free up resources no longer used
	// ----------------------------------

	free(devices);
}

static uint8_t validateDevice(VkPhysicalDevice device) {

	// -----------------------
	// Get device properties
	// -----------------------

	/// Properties check
	// Basic info about the device like name, API version, 
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(device, &properties);

	// ---------------
	// Version check
	// ---------------

	// We require methods only available in version 1.3 and above, so devices that can't handle those calls are essentially blocks of meaningless silicon to us
	if (properties.apiVersion < VK_API_VERSION_1_3) {
		log_info("Device \"%s\" determined unsuitable due to API version support\n", properties.deviceName);
		return UNSUITABLE; 
	}

	// -----------------------------
	// Get device queue properties
	// -----------------------------

	/// Scanning queue families for a valid family (supports graphics commands)
	uint32_t queuePropertiesCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queuePropertiesCount, NULL);
	VkQueueFamilyProperties* queueProperties = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queuePropertiesCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queuePropertiesCount, queueProperties);

	// --------------------------
	// Check family is sutiable
	// --------------------------

	uint8_t validFamily = FALSE;
	// Fun and fast-ish run through of all the devices, 90% of cases only 1-4 families on a device via trust me bro benchmark
	for (uint32_t i = 0; i < queuePropertiesCount; i++) {
		validFamily |= (queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT);
	}
	if (validFamily == FALSE) {
		log_warn("Device  \"%s\" has no queue family supporting graphics, unsuitable\n", properties.deviceName);
		return UNSUITABLE;
	}

	// ---------------------------
	// Get the device extensions
	// ---------------------------

	// Standard fetching steps
	uint32_t numExtensions = 0;
	VkExtensionProperties* extensionProperties = NULL;
	vkEnumerateDeviceExtensionProperties(device, NULL, &numExtensions, NULL);
	extensionProperties = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * numExtensions);
	vkEnumerateDeviceExtensionProperties(device, NULL, &numExtensions, extensionProperties);


	// -------------------------------
	// Perform search for extensions
	// -------------------------------

	// Names are all known at compile time
	const char* requiredExtensions[1] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	ExtensionSearchResult searchRes = searchExtensions(numExtensions, extensionProperties, 1, requiredExtensions);
	if (searchRes == MISSING_EXTENSIONS) {
		log_warn("Device \"%s\" is missing required extensions, unsuitable\n", properties.deviceName);
		return UNSUITABLE;
	}

	// -----------------------------
	// Setup all required features
	// -----------------------------

	/// Checking for all required features of the device, order must be bottom->top to chain pNexts
	// Device support info specific to the fancy dynamic state extensions
	VkPhysicalDeviceExtendedDynamicStateFeaturesEXT deviceDynamicFeatures;
	deviceDynamicFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_FEATURES_EXT;
	deviceDynamicFeatures.pNext = NULL;
	// Device support info specific to version 1.3
	VkPhysicalDeviceVulkan13Features device13Features;
	device13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
	device13Features.pNext = &deviceDynamicFeatures;
	// Device support info specific to version 1.1
	VkPhysicalDeviceVulkan11Features device11Features;
	device11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	device11Features.pNext = &device13Features;
	// Some basic information about the device features
	VkPhysicalDeviceFeatures2 deviceBaseFeatures;
	deviceBaseFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	deviceBaseFeatures.pNext = &device11Features;

	// ----------------------------
	// Get and check the features
	// ----------------------------

	// Fetching from the device
	vkGetPhysicalDeviceFeatures2(device, &deviceBaseFeatures);

	uint32_t bFeaturesPresent = device11Features.shaderDrawParameters & device13Features.dynamicRendering & deviceDynamicFeatures.extendedDynamicState;
	if (!bFeaturesPresent) {
		log_warn("Device \"%s\" is missing required features\n", properties.deviceName);
		return UNSUITABLE;
	}

	// --------------------------
	// Free up unused resources
	// --------------------------

	free(queueProperties);
	free(extensionProperties);

	return SUITABLE;
}
