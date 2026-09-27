#include "helpers/vulkanDebug.h"
#include "helpers/window.h"


int main(void) {
	// --------------------------------------------------
	// Setup of everything needed to render our scene
	// --------------------------------------------------
	
	// Makes sure everything behind it is going to play ball and create a window
	Window window = windowInit();

	VkInstance instance = NULL;
	// createInstance();
	
	
	VkDebugUtilsMessengerEXT debugMessenger = NULL;
	setupDebugMessaging(instance, &debugMessenger);


	// --------------------------------------------------
	// Primary 'game' loop (logic, rendering, input, etc)
	// --------------------------------------------------
	
	while (windowShouldClose(window)) {
	
		// Close events, input, etc
		windowPollEvents();
	}


	// --------------------------------------------------
	// Closing out everything nice and cleanly
	// --------------------------------------------------
	
	windowDestroy(window);
	return 0;
}
