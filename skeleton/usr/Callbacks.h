#include "DEFS.h"

class FractalCallbacks : public CallbackInterface {

public:
	// Constructor now takes the control struct by reference
	FractalCallbacks(ShaderProgram& shader, FractalControl& control)
		: shader(shader), control(control) {
	}

	// DEPTH IS LIMITED TO PREVENT CRASHES (MAX = 8)
	virtual void keyCallback(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) 
		{
			if (key == GLFW_KEY_R) 
			{
				shader.recompile();
			}

			// increase depth
			if (key == GLFW_KEY_UP) 
			{
				control.CurrentDepth++;
				// clamp to prevent crash
				if (control.CurrentDepth > 8) control.CurrentDepth = 8;
				control.bNeedsUpdate = true;
				std::cout << "Depth -> " << control.CurrentDepth << std::endl;
			}

			// decrease depth
			if (key == GLFW_KEY_DOWN) 
			{
				control.CurrentDepth--;
				if (control.CurrentDepth < 0) control.CurrentDepth = 0;
				control.bNeedsUpdate = true;
				std::cout << "Depth ->" << control.CurrentDepth << std::endl;
			}
		}
	}

private:
	ShaderProgram& shader;
	FractalControl& control;
};