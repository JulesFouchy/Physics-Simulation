#include <Cool/Log/Log.h>
#include <Cool/App/AppManager.h>
#include <Cool/App/OpenGLWindowingSystem.h>
#include <Cool/Time/Time.h>

#include "App.h"

int main() {
	// Init
	Cool::Log::Initialize();
	// Init Glfw and set OpenGL version to 4.3
	Cool::OpenGLWindowingSystem windowingSystem(4, 3);
	// Create the main window and its OpenGL context
	Cool::OpenGLWindow mainWindow = windowingSystem.createWindow("Physics", 1280, 720);
#ifdef NDEBUG
	glfwMaximizeWindow(mainWindow.get());
#endif
	// App
	App app(mainWindow);
	Cool::AppManager appManager(mainWindow, app);
	Cool::Time::Initialize();
	appManager.run();
}