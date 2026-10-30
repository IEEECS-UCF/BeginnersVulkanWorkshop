#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vulkan/vulkan_core.h>
#include "swapchain.h"

typedef enum {
	GRAPHICS_PIPELINE_SUCCESS,
} GraphicsPipelineResult;


GraphicsPipelineResult createGraphicsPipeline(VkDevice device, Swapchain* swapchain);


#endif
