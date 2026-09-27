#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>


// Just make it more abstract so there's less questions, behaves identically
typedef GLFWwindow* Window;

Window windowInit();

int windowShouldClose(Window window);

void windowPollEvents();

void windowDestroy(Window window);


#endif
