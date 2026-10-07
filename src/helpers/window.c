#include "window.h"
#include "log.h"

#include <stdlib.h>

GLFWwindow* windowInit(void) {
	if (glfwInit()) {
		log_debg("GLFW successfully initialized\n");
	} else {
		log_fail("GLFW initialization failed!\n");
		exit(1);
	}

	if (glfwVulkanSupported()) {
		log_ok("Vulkan is available\n");
	} else {
		log_fail("GLFW cannot find Vulkan or it is unsupported!\n");
		glfwTerminate();
		exit(1);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkanization!", NULL, NULL);
	if (!window) {
		log_fail("Window creation has failed!\n");
		glfwTerminate();
		exit(1);
	}
	log_ok("Window has been created\n"); // Wayland wont show anything for awhile

	return window;
}

inline int windowShouldClose(Window window) {return glfwWindowShouldClose(window);}

inline void windowPollEvents(void) {glfwPollEvents();}

void windowDestroy(GLFWwindow *window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}
