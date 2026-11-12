#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vulkan/vulkan_core.h>
#include "swapchain.h"

typedef enum {
	GRAPHICS_PIPELINE_SUCCESS,
	GRAPHICS_PIPELINE_INPUTS_UNINITIALIZED,
	GRAPHICS_PIPELINE_FAILURE,
} GraphicsPipelineResult;

typedef struct {
	VkPipeline handle;
	VkShaderModule shaderModule;
	VkPipelineLayout layout;
} GraphicsPipeline;
#define EMPTY_PIPELINE_OBJECT (GraphicsPipeline){NULL, NULL, NULL}


GraphicsPipelineResult createGraphicsPipeline(VkDevice device, Swapchain* swapchain, GraphicsPipeline* pipeline);


#endif
