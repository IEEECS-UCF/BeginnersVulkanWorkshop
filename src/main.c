// General purpose imports
#include <stdint.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "helpers/log.h"

// Action specific imports, in order of usage
#include "helpers/window.h"
#include "instance.h"
#include "helpers/vulkanDebug.h"
#include "physicalDevice.h"
#include "logicalDevice.h"
#include "swapchain.h"


static void destroyAll(Window window, VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkDevice logicalDevice, VkSurfaceKHR surface, Swapchain swapchain);


int main(void) {
	// --------------------------------------------------
	// Setup of everything needed to render our scene
	// --------------------------------------------------
	
	// Makes sure everything behind it is going to play ball and create a window
	Window window = windowInit();


	VkInstance instance = NULL;
	createInstance(&instance);


	VkDebugUtilsMessengerEXT debugMessenger = NULL;
	setupDebugMessaging(instance, &debugMessenger);


	VkSurfaceKHR surface = NULL;
	if (window && instance && glfwCreateWindowSurface(instance, window, NULL, &surface) != 0) {
		log_fail("Cannot create window surface\n");
		destroyAll(window, instance, debugMessenger, NULL, surface, EMPTY_SWAPCHAIN_OBJECT);
		return -1;
	}


	VkPhysicalDevice physicalDevice = NULL;
	selectPhysicalDevice(instance, &physicalDevice);


	VkDevice device = NULL;
	VkQueue graphicsQueue = NULL; // _technically_ also a present queue, but in our specific use case this is quibble
	uint32_t graphicsQueueIndex = 0;
	createLogicalDevice(physicalDevice, &device, surface, &graphicsQueue, &graphicsQueueIndex);


	Swapchain swapchain = EMPTY_SWAPCHAIN_OBJECT;
	createSwapchain(physicalDevice, device, surface, window, &swapchain);
	createImageViews(device, &swapchain);


	// --------------------------------------------------
	// Primary 'game' loop (logic, rendering, input, etc)
	// --------------------------------------------------
	
	while (windowShouldClose(window)) {
	
		// Close events, input, etc
		windowPollEvents();
	}


	// --------------------------------------------------
	// Closing out everything nice and cleanly
	// --------------------------------------------------
	
	destroyAll(window, instance, debugMessenger, device, surface, swapchain);
	return 0;
}


static void destroyAll(GLFWwindow* window, VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkDevice logicalDevice, VkSurfaceKHR surface, Swapchain swapchain) {
	if (swapchain.images.imageHandles != NULL) {
		// Images are destroyed with the swapchain, so no destroying them here
		for (uint32_t i = 0; i < swapchain.images.num; i++) {
			vkDestroyImageView(logicalDevice, swapchain.images.viewHandles[i], NULL);
		}
	}
	if (swapchain.handle != NULL) {
		vkDestroySwapchainKHR(logicalDevice, swapchain.handle, NULL);
	}
	if (surface != NULL)
		vkDestroySurfaceKHR(instance, surface, NULL);
	if (logicalDevice != NULL)
		vkDestroyDevice(logicalDevice, NULL);
	
	if (debugMessenger != NULL) {
		PFN_vkDestroyDebugUtilsMessengerEXT pfnDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)glfwGetInstanceProcAddress(instance, "vkDestroyDebugUtilsMessengerEXT");
		pfnDestroyDebugUtilsMessenger(instance, debugMessenger, NULL);
	}
	
	if (instance != NULL)
		vkDestroyInstance(instance, NULL);
	if (window != NULL)
		windowDestroy(window);
}
