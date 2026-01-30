#include "DEFS.h"

class FractalCallbacks : public CallbackInterface {

public:
	// Constructor now takes the control struct by reference
	FractalCallbacks(ShaderProgram& shader, FractalControl& control)
		: shader(shader), control(control) {
	}

	virtual void keyCallback(int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS || action == GLFW_REPEAT) 
		{
			if (key == GLFW_KEY_R) 
			{
				shader.recompile();
			}

			if (key == GLFW_KEY_1)
			{
				control.CurrentScene = SceneType::SIERPINSKY;
				control.bNeedsUpdate = true;
				std::cout << "Scene -> SIERPINSKY" << std::endl;
			}
			if (key == GLFW_KEY_2)
			{
				control.CurrentScene = SceneType::ANTI_SNOWFLAKE;
				control.bNeedsUpdate = true;
				std::cout << "Scene -> ANTI_SNOWFLAKE" << std::endl;
			}
			if (key == GLFW_KEY_3)
			{
				control.CurrentScene = SceneType::TREE;
				control.bNeedsUpdate = true;
				std::cout << "Scene -> TREE" << std::endl;
			}

			constexpr int MAX_DEPTH = 11;
			// increase depth
			if (key == GLFW_KEY_UP) 
			{
				control.CurrentDepth++;
				// clamp to prevent crash
				if (control.CurrentDepth > MAX_DEPTH) control.CurrentDepth = MAX_DEPTH;
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