#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Window.h"
#include "AssetPath.h"

#include "usr/DEFS.h"
#include "usr/Callbacks.h"

// EXAMPLE CALLBACKS
//class MyCallbacks : public CallbackInterface {
//
//public:
//	MyCallbacks(ShaderProgram& shader) : shader(shader) {}
//
//	virtual void keyCallback(int key, int scancode, int action, int mods) 
//	{
//		if (key == GLFW_KEY_R && action == GLFW_PRESS) { // On a positive edge press (when FIRST clicked)
//			shader.recompile();
//		}
//	}
//
//	// Other callbacks we implemented that you may use
//	//virtual void mouseButtonCallback(int button, int action, int mods) {}
//	//virtual void cursorPosCallback(double xpos, double ypos) {}
//	//virtual void scrollCallback(double xoffset, double yoffset) {}
//	//virtual void windowSizeCallback(int width, int height) { CallbackInterface::windowSizeCallback(width, height);/*Should be called*/ }
//
//private:
//	ShaderProgram& shader;
//};
//
//class MyCallbacks2 : public CallbackInterface {
//
//public:
//	MyCallbacks2() {}
//
//	virtual void keyCallback(int key, int scancode, int action, int mods) {
//		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
//			std::cout << "called back" << std::endl;
//		}
//	}
//};
// END EXAMPLES


void SierpinskiRecursion_SubTriangle(CPU_Geometry& _CPUGeom, Triangle _TRI, int _D)
{
	using namespace glm;

	if (_D == 0)
	{
		_CPUGeom.verts.push_back(_TRI.v0);
		_CPUGeom.verts.push_back(_TRI.v1);
		_CPUGeom.verts.push_back(_TRI.v2);

		_CPUGeom.cols.push_back(_TRI.col);
		_CPUGeom.cols.push_back(_TRI.col);
		_CPUGeom.cols.push_back(_TRI.col);
	}
	else
	{
		vec3 A = _TRI.v1; // Top
		vec3 B = _TRI.v0; // Left
		vec3 C = _TRI.v2; // Right

		// midpoints
		vec3 D = vec3((_TRI.v0.x + _TRI.v1.x) / 2, (_TRI.v0.y + _TRI.v1.y) / 2, 0.0f);
		vec3 E = vec3((_TRI.v1.x + _TRI.v2.x) / 2, (_TRI.v1.y + _TRI.v2.y) / 2, 0.0f);
		vec3 F = vec3((_TRI.v0.x + _TRI.v2.x) / 2, (_TRI.v0.y + _TRI.v2.y) / 2, 0.0f);

		int NextDepth = _D - 1;

		glm::vec3 ColTop = (_TRI.col + glm::vec3(1.0f, 0.0f, 0.0f)) * 0.5f;
		glm::vec3 ColLeft = (_TRI.col + glm::vec3(0.0f, 1.0f, 0.0f)) * 0.5f;
		glm::vec3 ColRight = (_TRI.col + glm::vec3(0.0f, 0.0f, 1.0f)) * 0.5f;
		SierpinskiRecursion_SubTriangle(_CPUGeom, Triangle(A, D, E, ColTop), NextDepth);
		SierpinskiRecursion_SubTriangle(_CPUGeom, Triangle(F, E, C, ColLeft), NextDepth);
		SierpinskiRecursion_SubTriangle(_CPUGeom, Triangle(B, D, F, ColRight), NextDepth);
	}
}

int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();//MUST call this first to set up environment (There is a terminate pair after the loop)

	{ // Extra scope to ensure all GL releated destructors are called (for example in Window class) before glfwTerminate

		Window window(800, 800, "Assignment 1: Fractals"); // Can set callbacks at construction if desired

		GLDebug::enable(); // ON Submission you may comments this out to avoid unnecessary prints to the console

		// SHADERS
		ShaderProgram shader(
			AssetPath::Instance()->Get("shaders/basic.vert"), 
			AssetPath::Instance()->Get("shaders/basic.frag")
		); // Render pipeline we will use (You can use more than one!)


		FractalControl fractalControl;
		fractalControl.CurrentDepth = 0; // Start at iteration 0

		// CALLBACKS
		std::shared_ptr<FractalCallbacks> callback_ptr = std::make_shared<FractalCallbacks>(shader, fractalControl);
		window.setCallbacks(callback_ptr);


		//// CALLBACKS
		//std::shared_ptr<MyCallbacks> callback_ptr = std::make_shared<MyCallbacks>(shader); // Class To capture input events
		////std::shared_ptr<MyCallbacks2> callback2_ptr = std::make_shared<MyCallbacks2>();
		//window.setCallbacks(callback_ptr); // Can also update callbacks to new ones as needed (create more than one instance)

		// GEOMETRY
		CPU_Geometry cpuGeom; // Just a collection of vectors
		GPU_Geometry gpuGeom; // Wrapper managing VAO and VBOs, in a TIGHTLY packed format
		//https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices#Attribute_sizes



		Triangle mainTriangle
		(
			glm::vec3(0.f, 0.5f, 0.f),
			glm::vec3(-0.5f, -0.5f, 0.f),
			glm::vec3(0.5f, -0.5f, 0.f),
			glm::vec3(0, 1, 0)
		);

		cpuGeom.verts.push_back(mainTriangle.v0);
		cpuGeom.verts.push_back(mainTriangle.v1);
		cpuGeom.verts.push_back(mainTriangle.v2);

		cpuGeom.cols.push_back(mainTriangle.col);
		cpuGeom.cols.push_back(mainTriangle.col);
		cpuGeom.cols.push_back(mainTriangle.col);

		SierpinskiRecursion_SubTriangle(cpuGeom, mainTriangle, fractalControl.CurrentDepth);

		//// vertices
		//cpuGeom.verts.push_back(glm::vec3(0.f, 0.5f, 0.f)); // Upper
		//cpuGeom.verts.push_back(glm::vec3(-0.5f, -0.5f, 0.f)); // Lower Left
		//cpuGeom.verts.push_back(glm::vec3(0.5f, -0.5f, 0.f)); // Lower Right

		//// colours (these should be in linear space)
		//cpuGeom.cols.push_back(glm::vec3(1.f, 0.f, 0.f)); // Red
		//cpuGeom.cols.push_back(glm::vec3(0.f, 1.f, 0.f)); // Green
		//cpuGeom.cols.push_back(glm::vec3(0.f, 0.f, 1.f)); // Blue



		gpuGeom.setVerts(cpuGeom.verts); // Upload vertex position geometry to VBO
		gpuGeom.setCols(cpuGeom.cols); // Upload vertex colour attribute to VBO

		// RENDER LOOP
		while (!window.shouldClose()) {
			glfwPollEvents(); // Propagate events to the callback class

			if (fractalControl.bNeedsUpdate) 
			{
				
				cpuGeom.verts.clear();
				cpuGeom.cols.clear();

				SierpinskiRecursion_SubTriangle(cpuGeom, mainTriangle, fractalControl.CurrentDepth);

				gpuGeom.setVerts(cpuGeom.verts);
				gpuGeom.setCols(cpuGeom.cols);

				fractalControl.bNeedsUpdate = false;
			}


			shader.use(); // Use "this" shader to render
			gpuGeom.bind(); // USe "this" VAO (Geometry) on render call

			glEnable(GL_FRAMEBUFFER_SRGB); // Expect Colour to be encoded in sRGB standard (as opposed to RGB) 
			// https://www.viewsonic.com/library/creative-work/srgb-vs-adobe-rgb-which-one-to-use/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear render screen (all zero) and depth (all max depth)
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)cpuGeom.verts.size()); // Render Triangle primatives, starting at index 0 (first) with a total of 3 elements (in this case 1 triangle)
			glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui (if used)

			window.swapBuffers(); //Swap the buffers while displaying the previous 
		}
		
	}

	glfwTerminate(); // Clean up GLFW
	return 0;
}
