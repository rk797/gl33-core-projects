#include "Window.h"

#include "Log.h"

#include <iostream>


// ---------------------------
// static function definitions
// ---------------------------

void Window::keyMetaCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	CallbackInterface* callbacks = static_cast<CallbackInterface*>(glfwGetWindowUserPointer(window));
	callbacks->keyCallback(key, scancode, action, mods);
}


void Window::mouseButtonMetaCallback(GLFWwindow* window, int button, int action, int mods) {
	CallbackInterface* callbacks = static_cast<CallbackInterface*>(glfwGetWindowUserPointer(window));
	callbacks->mouseButtonCallback(button, action, mods);
}


void Window::cursorPosMetaCallback(GLFWwindow* window, double xpos, double ypos) {
	CallbackInterface* callbacks = static_cast<CallbackInterface*>(glfwGetWindowUserPointer(window));
	callbacks->cursorPosCallback(xpos, ypos);
}


void Window::scrollMetaCallback(GLFWwindow* window, double xoffset, double yoffset) {
	CallbackInterface* callbacks = static_cast<CallbackInterface*>(glfwGetWindowUserPointer(window));
	callbacks->scrollCallback(xoffset, yoffset);
}


void Window::windowSizeMetaCallback(GLFWwindow* window, int width, int height) {
	CallbackInterface* callbacks = static_cast<CallbackInterface*>(glfwGetWindowUserPointer(window));
	callbacks->windowSizeCallback(width, height);
}

void Window::framebufferSizeMetaCallback(GLFWwindow* window, int width, int height) {
	CallbackInterface* callbacks = static_cast<CallbackInterface*>(glfwGetWindowUserPointer(window));
	callbacks->framebufferSizeCallback(width, height);
}

// ----------------------
// non-static definitions
// ----------------------

Window::Window(
	std::shared_ptr<CallbackInterface> callbacks, int width, int height,
	const char* title, int msaaSamplesHint, GLFWmonitor* monitor, GLFWwindow* share
)
	: window(nullptr)
	, callbacks(callbacks)
	, msaaSamplesHint(msaaSamplesHint)
{
	// specify OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // needed for mac?
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	if (msaaSamplesHint > 0) {
		// For multisampling/anti-aliasing
		glfwWindowHint(GLFW_SAMPLES, msaaSamplesHint);
	}


	// create window
	window = std::unique_ptr<GLFWwindow, WindowDeleter>(glfwCreateWindow(width, height, title, monitor, share));
	if (window == nullptr) {
		Log::error("WINDOW failed to create GLFW window");
		throw std::runtime_error("Failed to create GLFW window.");
	}
	glfwMakeContextCurrent(window.get());

	// initialize OpenGL extensions for the current context (this window)
	if (!gladLoadGL()) {
		throw std::runtime_error("Failed to initialize GLAD");
	}

	// This is another call we may need to make to enable multisampling.
	// This call is done *after* we construct our window, because window
	// creation is what creates an OpenGL "context" for us to modify. 
	// If you're interested in what the definition of a "context" is here,
	// see: https://wikis.khronos.org/opengl/OpenGL_Context
	if (msaaSamplesHint > 0) {
		glEnable(GL_MULTISAMPLE);
	}

	// If no callbacks were passed in, then we create & set default ones.
	if (callbacks == nullptr) {
		this->callbacks = std::make_shared<CallbackInterface>();
	}

	connectCallbacks();

}


Window::Window(int width, int height, const char* title, int msaaSamplesHint, GLFWmonitor* monitor, GLFWwindow* share)
	: Window(nullptr, width, height, title, msaaSamplesHint, monitor, share)
{}

Window::~Window() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

// Boilerplate ImGui setup code. Informed by:
// https://github.com/ocornut/imgui/blob/master/examples/example_glfw_opengl3/main.cpp
void Window::setupImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// We call GetIO() and then use a void cast to supress compiler warnings
	// about unused variables, since we don't use io.
	// We just want the IM_ASSERT in GetIO() to be run, I guess.
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Uncomment the below if you want to use, e.g., arrow keys to navigate
	// between ImGui widgets.
	// 
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Sets dark colour style.
	// You can use StyleColorsLight() here instead if you want.
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.get(), false);

	// Here, we pass in the glsl version we are using, which should line up
	// with our OpenGL version. There's not much documentation on this ImGui
	// function, but from what I can tell from other people's comments,
	// tutorials, and reading through imgui_impl_opengl3.cpp myself, this
	// seems to be the right approach.
	ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Window::connectCallbacks() {
	// set userdata of window to point to the object that carries out the callbacks
	glfwSetWindowUserPointer(window.get(), callbacks.get());

	// bind meta callbacks to actual callbacks
	glfwSetKeyCallback(window.get(), keyMetaCallback);
	glfwSetMouseButtonCallback(window.get(), mouseButtonMetaCallback);
	glfwSetCursorPosCallback(window.get(), cursorPosMetaCallback);
	glfwSetScrollCallback(window.get(), scrollMetaCallback);
	glfwSetWindowSizeCallback(window.get(), windowSizeMetaCallback);
	glfwSetFramebufferSizeCallback(window.get(), framebufferSizeMetaCallback);
}


void Window::setCallbacks(std::shared_ptr<CallbackInterface> callbacks_) {
	callbacks = callbacks_;
	connectCallbacks();
}


glm::ivec2 Window::getPos() const {
	int x, y;
	glfwGetWindowPos(window.get(), &x, &y);
	return glm::ivec2(x, y);
}


glm::ivec2 Window::getSize() const {
	int w, h;
	glfwGetWindowSize(window.get(), &w, &h);
	return glm::ivec2(w, h);
}
