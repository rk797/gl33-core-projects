#include "DEFS.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
class FractalCallbacks : public CallbackInterface {

public:
	// Constructor now takes the control struct by reference
	FractalCallbacks(ShaderProgram& shader, FractalControl& control)
		: shader(shader), control(control) {
	}

	virtual void mouseButtonCallback(int button, int action, int mods) override
	{
		ImGui_ImplGlfw_MouseButtonCallback(glfwGetCurrentContext(), button, action, mods);

		if (ImGui::GetIO().WantCaptureMouse)
		{
			return;
		}
	}

	virtual void cursorPosCallback(double xpos, double ypos) override
	{
		ImGui_ImplGlfw_CursorPosCallback(glfwGetCurrentContext(), xpos, ypos);
	}

	virtual void scrollCallback(double xoffset, double yoffset) override
	{
		ImGui_ImplGlfw_ScrollCallback(glfwGetCurrentContext(), xoffset, yoffset);
	}


	virtual void keyCallback(int key, int scancode, int action, int mods)
	{

		ImGui_ImplGlfw_KeyCallback(glfwGetCurrentContext(), key, scancode, action, mods);
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

			// increase depth
			if (key == GLFW_KEY_UP) 
			{
				control.CurrentDepth++;
				// clamp to prevent crash
				if (control.CurrentDepth > control.MaxDepth) control.CurrentDepth = control.MaxDepth;
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