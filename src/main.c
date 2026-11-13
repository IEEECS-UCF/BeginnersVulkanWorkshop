// General purpose imports
#include "graphicsPipeline.h"
#include <stdint.h>
#include <stdlib.h>
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
#include "commandBuffer.h"
#include "draw.h"


static void destroyAll(
	Window window, VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, 
	VkDevice logicalDevice, VkSurfaceKHR surface, Swapchain swapchain, 
	GraphicsPipeline pipeline, VkCommandPool commandPool, SyncObjects sync);


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
		destroyAll(window, instance, debugMessenger, NULL, surface, EMPTY_SWAPCHAIN_OBJECT, EMPTY_PIPELINE_OBJECT, NULL, EMPTY_SYNC_OBJECTS);
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


	GraphicsPipeline graphicsPipeline = EMPTY_PIPELINE_OBJECT;
	createGraphicsPipeline(device, &swapchain, &graphicsPipeline);


	VkCommandPool commandPool = NULL;
	createCommandPool(device, graphicsQueueIndex, &commandPool);


	VkCommandBuffer commandBuffer = NULL;
	createCommandBuffer(device, commandPool, &commandBuffer);


	SyncObjects sync = EMPTY_SYNC_OBJECTS;
	createSynchronizationObjects(device, &sync, swapchain.images.num);

	// --------------------------------------------------
	// Primary 'game' loop (logic, rendering, input, etc)
	// --------------------------------------------------
	
	// Cheeky break to only draw one frame
	while (!windowShouldClose(window)) {
		drawFrame(device, sync, swapchain, commandBuffer, graphicsQueue, graphicsPipeline.handle);

		// Close events, input, etc
		windowPollEvents();
		break;
	}


	// --------------------------------------------------
	// Closing out everything nice and cleanly
	// --------------------------------------------------
	
	if (device) { vkDeviceWaitIdle(device); } // Ensure work has completed so we're not bailing out mid-GPU-operation
	destroyAll(window, instance, debugMessenger, device, surface, swapchain, graphicsPipeline, commandPool, sync);
	return 0;
}


static void destroyAll(
	GLFWwindow* window, 
	VkInstance instance, 
	VkDebugUtilsMessengerEXT debugMessenger, 
	VkDevice logicalDevice, 
	VkSurfaceKHR surface, 
	Swapchain swapchain,
	GraphicsPipeline pipeline,
	VkCommandPool commandPool,
	SyncObjects sync
) {
	if (sync.drawFence != NULL) {
		vkDestroyFence(logicalDevice, sync.drawFence, NULL);
		vkDestroySemaphore(logicalDevice, sync.presentComplete, NULL);
		for (uint32_t i = 0; i < swapchain.images.num; i++) {
			vkDestroySemaphore(logicalDevice, sync.renderComplete[i], NULL);
		}
	}

	if (commandPool != NULL)
		vkDestroyCommandPool(logicalDevice, commandPool, NULL);

	if (pipeline.shaderModule)
		vkDestroyShaderModule(logicalDevice, pipeline.shaderModule, NULL);

	if (pipeline.layout != NULL) 
		vkDestroyPipelineLayout(logicalDevice, pipeline.layout, NULL);

	if (pipeline.handle != NULL) 
		vkDestroyPipeline(logicalDevice, pipeline.handle, NULL);

	if (swapchain.images.viewHandles != NULL) {
		// Images are destroyed with the swapchain, so no destroying them here
		for (uint32_t i = 0; i < swapchain.images.num; i++) {
			vkDestroyImageView(logicalDevice, swapchain.images.viewHandles[i], NULL);
		}
	}

	if (swapchain.handle != NULL) 
		vkDestroySwapchainKHR(logicalDevice, swapchain.handle, NULL);

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
