#pragma once

//------------------------------------------------------------------------------
// This file contains classes that provide a simpler and safer interface for
// interacting with a GLFW window following RAII principles
//------------------------------------------------------------------------------

// Multiple tutorials/examples include ImGui before glad/GLFW. I do not know
// what the benefit is, if one even exists, though.
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>


// Class that specifies the interface for the most common GLFW callbacks
//
// These are the default implementations. You can write your own class that
// extends this one and overrides the implementations with your own
class CallbackInterface {
public:
	virtual void keyCallback(int key, int scancode, int action, int mods) {}
	virtual void mouseButtonCallback(int button, int action, int mods) {}
	virtual void cursorPosCallback(double xpos, double ypos) {}
	virtual void scrollCallback(double xoffset, double yoffset) {}

	// Takes in screen coordinates of the window, the same coordinate system
	// in which cursorPosCallback() works. So this should be used to change
	// how cursor positions are interpreted, but NOT for glViewport(), which
	// should be handled by frameBufferSizeCallback() instead, as that uses
	// actual pixel coordinates.
	virtual void windowSizeCallback(int width, int height) {}

	// While other callbacks' default implementations do nothing, the default
	// framebuffer size callback should resize the OpenGL viewport.
	virtual void framebufferSizeCallback(int width, int height) {
		glViewport(0, 0, width, height);
	}
};


// Functor for deleting a GLFW window.
//
// This is used as a custom deleter with std::unique_ptr so that the window
// is properly destroyed when std::unique_ptr needs to clean up its resource
struct WindowDeleter {
	void operator() (GLFWwindow* window) const {
		glfwDestroyWindow(window);
	}
};


// Main class for creating and interacting with a GLFW window.
// Only wraps the most fundamental parts of the API
class Window {

public:
	Window(
		std::shared_ptr<CallbackInterface> callbacks, int width, int height,
		const char* title, int msaaSamplesHint = 0, GLFWmonitor* monitor = NULL,
		GLFWwindow* share = NULL
	);
	Window(
		int width, int height, const char* title, int msaaSamplesHint = 0,
		GLFWmonitor* monitor = NULL, GLFWwindow* share = NULL
	);

	~Window();

	void setCallbacks(std::shared_ptr<CallbackInterface> callbacks);

	glm::ivec2 getPos() const;
	glm::ivec2 getSize() const;

	int getX() const { return getPos().x; }
	int getY() const { return getPos().y; }
	
	int getWidth() const { return getSize().x; }
	int getHeight() const { return getSize().y; }

	int shouldClose() { return glfwWindowShouldClose(window.get()); }
	void makeContextCurrent() { glfwMakeContextCurrent(window.get()); }
	void swapBuffers() { glfwSwapBuffers(window.get()); }

	GLFWwindow* getGLFWwindow() const { return window.get(); }

private:

	void setupImGui();

	std::unique_ptr<GLFWwindow, WindowDeleter> window; // owning ptr (from GLFW)
	std::shared_ptr<CallbackInterface> callbacks;      // optional shared owning ptr (user provided)
	int msaaSamplesHint;

	void connectCallbacks();

	// Meta callback functions. These bind to the actual glfw callback,
	// get the actual callback method from user data, and then call that.
	static void keyMetaCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseButtonMetaCallback(GLFWwindow* window, int button, int action, int mods);
	static void cursorPosMetaCallback(GLFWwindow* window, double xpos, double ypos);
	static void scrollMetaCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void windowSizeMetaCallback(GLFWwindow* window, int width, int height);
	static void framebufferSizeMetaCallback(GLFWwindow* window, int width, int height);
};

