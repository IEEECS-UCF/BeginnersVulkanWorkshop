#ifndef SEARCHING_H
#define SEARCHING_H

#include <stdint.h>
#include <vulkan/vulkan_core.h>

typedef enum {
	ALL_EXTENSIONS_PRESENT,
	MISSING_EXTENSIONS,
} ExtensionSearchResult;


 ExtensionSearchResult searchExtensions(uint32_t extensionsSize, VkExtensionProperties* extensions, uint32_t targetSize, const char* const* targets);

#endif
