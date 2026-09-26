#include <stdio.h>

#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

int main(void) {
	if (glfwInit()) {
		printf("GLFW successfully initialized!\n");
	} else {
		printf("GLFW initialization failed!\n");
		exit(1);
	}

	if (glfwVulkanSupported()) {
		printf("Vulkan is available!\n");
	} else {
		printf("GLFW cannot find Vulkan or it is unsupported!\n");
		glfwTerminate();
		exit(1);
	}

	glfwTerminate();
	return 0;
}
