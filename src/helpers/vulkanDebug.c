#include "helpers/log.h"
#include <stdio.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagBitsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData);


void setupDebugMessaging(VkInstance instance, VkDebugUtilsMessengerEXT* messenger) {
	if (!instance) // For when the instance is undeveloped
		log_warn("Instance passed to debug messaging uninitalized\n");
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = NULL,
		.flags = 0, // Untouched forever, just axe it Khronos lmao
		.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
			//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
		.messageType = 
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
			//VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = debugCallback,
		.pUserData = NULL,
	};

	// Becuase its an extension we have to load the create function in at runtime
	PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)glfwGetInstanceProcAddress(
			instance,
			"vkCreateDebugUtilsMessengerEXT"
	);

	VkResult res = pfnVkCreateDebugUtilsMessengerEXT(
			instance,
			&createInfo,
			NULL,
			messenger
	);
	if (res == VK_SUCCESS)
		log_debg("Primary Vulkan debug messenger created\n");
	else
		log_warn("Unable to create primary Vulkan debug messenger\n");
}

inline VkDebugUtilsMessengerCreateInfoEXT instanceCreationDebugHook() {
	return (VkDebugUtilsMessengerCreateInfoEXT){
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.pNext = NULL,
		.flags = 0, 
		.messageSeverity = 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
		.messageType = 
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
			//VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = debugCallback,
		.pUserData = NULL,
	};
}


// Index is based off of bitmask spec
const char* messageTypeStrings[4] = {
	"\x1b[38;5;129m[GENERAL]",
	"\x1b[38;5;217m[VALIDATION]",
	"",
	"\x1b[38;5;222m[PERFORMANCE]",
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagBitsEXT type,
		const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
		void* userData
) {
	fputs(messageTypeStrings[type], stdout);
	switch (severity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			break; // Guarenteed not to be sent to us based on the contract established at creation

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			log_info("%s\n", callbackData->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			log_warn("%s\n", callbackData->pMessage);
			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			log_fail("%s\n", callbackData->pMessage);
			break;
	}
	return VK_FALSE;
}
