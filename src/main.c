#include "window.h"

int main(void) {
	// --------------------------------------------------
	// Setup of everything needed to render our scene
	// --------------------------------------------------
	
	// Makes sure everything behind it is going to play ball and create a window
	Window window = windowInit();


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
