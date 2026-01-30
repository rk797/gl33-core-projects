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
#include <cmath>

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

#define PI 3.14159265358979323846

void RotateVector(glm::vec3& vec, float angleDegrees)
{
	float angRadians = angleDegrees * PI / 180.0;

	float cosTheta = cos(angRadians);
	float sinTheta = sin(angRadians);

	float x = vec.x * cosTheta - vec.y * sinTheta;
	float y = vec.x * sinTheta + vec.y * cosTheta;
	vec.x = x;
	vec.y = y;
}
void KockRecursion_AntiSnowflake(CPU_Geometry& _CPUGeom, glm::vec3 _Start, glm::vec3 _End, glm::vec3 _StartCol, glm::vec3 _EndCol, int _D)
{
	using namespace glm;

	if (_D == 0)
	{
		_CPUGeom.verts.push_back(_Start);
		_CPUGeom.cols.push_back(_StartCol);
	}
	else
	{
		
		vec3 A = _Start;
		vec3 B = _End;
		// 1/3 of the way from the line going from points A -> B
		vec3 m1 = vec3((2 * A.x + B.x) / 3, (2 * A.y + B.y) / 3, 0.0f);
		// 2/3 of the way from the line going from points A -> B
		vec3 m2 = vec3((A.x + 2 * B.x) / 3, (A.y + 2 * B.y) / 3, 0.0f);

		// interpolate colors at m1 and m2
		vec3 colM1 = (_StartCol * 2.0f + _EndCol) / 3.0f;
		vec3 colM2 = (_StartCol + _EndCol * 2.0f) / 3.0f;

		vec3 v = m2 - m1;

		RotateVector(v, 60.0f);

		vec3 peak = v + m1;
		vec3 colPeak = (colM1 + colM2) / 2.0f;

		int NextDepth = _D - 1;

		KockRecursion_AntiSnowflake(_CPUGeom, A, m1, _StartCol, colM1, NextDepth);
		KockRecursion_AntiSnowflake(_CPUGeom, m1, peak, colM1, colPeak, NextDepth);
		KockRecursion_AntiSnowflake(_CPUGeom, peak, m2, colPeak, colM2, NextDepth);
		KockRecursion_AntiSnowflake(_CPUGeom, m2, B, colM2, _EndCol, NextDepth);
	}
}

//void AddLine(glm::vec3 _Start, glm::vec3 _End)

void TreeRecursion_Branch(CPU_Geometry& _CPUGeom, glm::vec3 _Start, glm::vec3 _End, int _D, bool bLastRotateDirection)
{
	// lambda function with capture by reference
	auto AddLine = [&](glm::vec3 s, glm::vec3 e) -> void
		{
			_CPUGeom.verts.push_back(s);
			_CPUGeom.verts.push_back(e);
			if (_D < 6)
			{
				glm::vec3 LeafColor(0.0f, 1.0f, 0.0f);
				_CPUGeom.cols.push_back(LeafColor);
				_CPUGeom.cols.push_back(LeafColor);
			}
			else
			{
				glm::vec3 BranchColor(0.55f, 0.27f, 0.07f);
				_CPUGeom.cols.push_back(BranchColor);
				_CPUGeom.cols.push_back(BranchColor);
			}
		};


	AddLine(_Start, _End);

	// bLastRotateDirection
	// 1 = right
	// 0 = left
	using namespace glm;
	if (_D  == 0)
	{
		return;
		
	}
	else
	{
		glm::vec3 v = _End - _Start;


		// get the point at 0.65 of the way using linear parameterization
		float alpha = 0.65f;
		glm::vec3 p65 = (1.0f - alpha) * _Start + alpha * _End;
		glm::vec3 v65 = p65 - _Start;
		glm::vec3 q1 = _End + v65; // the point at 0.65 of the way extended beyond the end point
		glm::vec3 q2 = q1 + v65; // the end of the main branch
		glm::vec3 q1v = q1 - _End; // the vector for the rotated branch
		RotateVector(q1v, bLastRotateDirection ? -27.5f : 27.5f);
		glm::vec3 q3 = q1v + _End;

		int NextDepth = _D - 1;
		bLastRotateDirection = !bLastRotateDirection;

		AddLine(_End, q1);
		TreeRecursion_Branch(_CPUGeom, q1, q2, NextDepth, bLastRotateDirection);
		TreeRecursion_Branch(_CPUGeom, _End, q3, NextDepth, !bLastRotateDirection);
	}

	


	//float len = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));


	// 
}



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


		glm::vec3 v0(0.0f, 0.366f, 0.0f);   // Top
		glm::vec3 v1(-0.5f, -0.5f, 0.0f);   // Left
		glm::vec3 v2(0.5f, -0.5f, 0.0f);    // Right


		glm::vec3 t1(0.0f, -0.6f, 0.0f);   // Bottom
		glm::vec3 t2(0.0f, -0.3f, 0.0f);    // Top

		Triangle mainTriangle(v0, v1, v2, glm::vec3(0, 1, 0));

		// RENDER LOOP
		while (!window.shouldClose()) 
		{
			glfwPollEvents(); // Propagate events to the callback class

			if (fractalControl.bNeedsUpdate) 
			{
				
				cpuGeom.verts.clear();
				cpuGeom.cols.clear();

				if (fractalControl.CurrentScene == SceneType::SIERPINSKY)
				{
					SierpinskiRecursion_SubTriangle(cpuGeom, mainTriangle, fractalControl.CurrentDepth);
				}
				else if (fractalControl.CurrentScene == SceneType::ANTI_SNOWFLAKE)
				{
					glm::vec3 blueCol(0.0f, 0.0f, 1.0f);
					glm::vec3 redCol(1.0f, 0.0f, 0.0f);
					glm::vec3 greenCol(0.0f, 1.0f, 0.0f);
					
					KockRecursion_AntiSnowflake(cpuGeom, v0, v1, blueCol, redCol, fractalControl.CurrentDepth);
					KockRecursion_AntiSnowflake(cpuGeom, v1, v2, redCol, greenCol, fractalControl.CurrentDepth);
					KockRecursion_AntiSnowflake(cpuGeom, v2, v0, greenCol, blueCol, fractalControl.CurrentDepth);
				}

				else if (fractalControl.CurrentScene == SceneType::TREE)
				{
					TreeRecursion_Branch(cpuGeom, t1, t2, fractalControl.CurrentDepth, 1);
				}

				gpuGeom.setVerts(cpuGeom.verts);
				gpuGeom.setCols(cpuGeom.cols);

				fractalControl.bNeedsUpdate = false;
			}


			shader.use(); // Use "this" shader to render
			gpuGeom.bind(); // USe "this" VAO (Geometry) on render call

			glEnable(GL_FRAMEBUFFER_SRGB); // Expect Colour to be encoded in sRGB standard (as opposed to RGB) 
			// https://www.viewsonic.com/library/creative-work/srgb-vs-adobe-rgb-which-one-to-use/
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear render screen (all zero) and depth (all max depth)

			if (fractalControl.CurrentScene == SceneType::SIERPINSKY)
			{
				glDrawArrays(GL_TRIANGLES, 0, (GLsizei)cpuGeom.verts.size()); // Render Triangle primatives, starting at index 0 (first) with a total of 3 elements (in this case 1 triangle)
			}
			else if (fractalControl.CurrentScene == SceneType::TREE)
			{
				// MUST use GL_LINES for the tree because it is a collection of segments
				// that are not all continuously connected in a single loop.
				glDrawArrays(GL_LINES, 0, (GLsizei)cpuGeom.verts.size());
			}
			else
			{
				glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)cpuGeom.verts.size());
			}
			
			glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui (if used)

			window.swapBuffers(); //Swap the buffers while displaying the previous 
		}
		
	}

	glfwTerminate(); // Clean up GLFW
	return 0;
}
