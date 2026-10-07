#ifndef VULKANDEBUG_H
#define VULKANDEBUG_H

#include <vulkan/vulkan_core.h>


void setupDebugMessaging(VkInstance instance, VkDebugUtilsMessengerEXT* messenger);

VkDebugUtilsMessengerCreateInfoEXT instanceCreationDebugHook(void);

#endif
