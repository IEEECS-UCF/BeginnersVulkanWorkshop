#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vulkan/vulkan_core.h>
#include "swapchain.h"

typedef struct {
	VkPipeline handle;
	VkShaderModule shaderModule;
	VkPipelineLayout layout;
} GraphicsPipeline;
#define EMPTY_PIPELINE_OBJECT (GraphicsPipeline){NULL, NULL, NULL}


void createGraphicsPipeline(VkDevice device, Swapchain* swapchain, GraphicsPipeline* pipeline);


#endif
