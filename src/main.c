#include <unistd.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "draw.h"
#include "commandBuffer.h"
#include "graphicsPipeline.h"
#include "helpers/vulkanDebug.h"
#include "helpers/window.h"
#include "instance.h"
#include "logicalDevice.h"
#include "physicalDevice.h"
#include "helpers/log.h"
#include "swapchain.h"
#include <stdlib.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>



static void destroyAll(GLFWwindow* window, VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkDevice logicalDevice, VkSurfaceKHR surface);


int main(void) {
	// --------------------------------------------------
	// Setup of everything needed to render our scene
	// --------------------------------------------------
	
	// Makes sure everything behind it is going to play ball and create a window
	Window window = windowInit();

	// FIXME: All these below need to have their errors handled AND be able to consume/recognize empty inputs without obliterating themselves

	VkInstance instance = NULL;
	createInstance(&instance);
	

	VkDebugUtilsMessengerEXT debugMessenger = NULL;
	setupDebugMessaging(instance, &debugMessenger);


	VkSurfaceKHR surface = NULL;
	if (window && instance && glfwCreateWindowSurface(instance, window, NULL, &surface) != 0) {
		log_fail("Cannot create window surface\n");
		destroyAll(window, instance, debugMessenger, NULL, surface);
		return -1;
	}


	VkPhysicalDevice physicalDevice = NULL;
	selectPhysicalDevice(instance, &physicalDevice);


	VkDevice device = NULL;
	VkQueue graphicsQueue = NULL; // _technically_ also a present queue, but in our specific use case this is quibble
	uint32_t graphicsQueueIndex = 0;
	createLogicalDevice(physicalDevice, &device, surface, &graphicsQueue, &graphicsQueueIndex);


	//
	Swapchain swapchain;
	createSwapchain(physicalDevice, device, surface, window, &swapchain);
	createImageViews(device, &swapchain);


	//
	VkPipeline graphicsPipeline = NULL;
	createGraphicsPipeline(device, &swapchain, &graphicsPipeline);

	VkCommandPool commandPool = NULL;
	createCommandPool(device, graphicsQueueIndex, &commandPool);

	VkCommandBuffer commandBuffer = NULL;
	createCommandBuffer(device, commandPool, &commandBuffer);
	//recordCommandBuffer(commandBuffer, swapchain, 0, graphicsPipeline);


	// Setup synchronization objects
	SyncObjects sync = EMPTY_SYNC_OBJECTS;
	createSynchronizationObjects(device, &sync);
	
	// --------------------------------------------------
	// Primary 'game' loop (logic, rendering, input, etc)
	// --------------------------------------------------
	
	while (!windowShouldClose(window)) {
	
		// Close events, input, etc
		windowPollEvents();
		drawFrame(device, &sync, swapchain, commandBuffer, graphicsQueue, graphicsPipeline);
	}


	// --------------------------------------------------
	// Closing out everything nice and cleanly
	// --------------------------------------------------
	
	vkDeviceWaitIdle(device); // Ensure work has completed so we're not bailing out mid-GPU-operation
	destroyAll(window, instance, debugMessenger, device, surface);
	return 0;
}


static void destroyAll(GLFWwindow* window, VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, VkDevice logicalDevice, VkSurfaceKHR surface) {
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
