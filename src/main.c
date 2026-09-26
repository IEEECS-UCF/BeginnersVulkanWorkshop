#include <stdlib.h>

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "log.h"

int main(void) {
	if (glfwInit()) {
		log_ok("GLFW successfully initialized!\n");
	} else {
		log_fail("GLFW initialization failed!\n");
		exit(1);
	}

	if (glfwVulkanSupported()) {
		log_ok("Vulkan is available!\n");
	} else {
		log_fail("GLFW cannot find Vulkan or it is unsupported!\n");
		glfwTerminate();
		exit(1);
	}

	glfwTerminate();
	return 0;
}
