#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Window.h"
#include "Panel.h"
#include "AssetPath.h"
#include "Curves.h"
#include <iostream>

enum class CurveMode
{
	Bezier,
	BSpline
};

class CurveEditorCallBack : public CallbackInterface {
private:
	std::vector<glm::vec3>* ControlPoints;
	GPU_Geometry* PointGpu;
	GPU_Geometry* LineGpu;
	GPU_Geometry* CurveGpu;
	int WindowWidth;
	int WindowHeight;
	glm::vec3 MousePosNdc{};
	int SelectedPointIndex = 0;
	bool IsDragging = false;

	CurveMode CurrentMode;
	int SubdivisionLevel = 0;
	int CurveVertexCount = 0;

	void UpdateBuffers()
	{
		PointGpu->setVerts(*ControlPoints);
		std::vector<glm::vec3> point_colors(ControlPoints->size(), glm::vec3(1.0f, 0.0f, 0.0f));
		PointGpu->setCols(point_colors);

		LineGpu->setVerts(*ControlPoints);
		std::vector<glm::vec3> line_colors(ControlPoints->size(), glm::vec3(0.0f, 1.0f, 0.0f));
		LineGpu->setCols(line_colors);

		std::vector<glm::vec3> generated_vertices;

		if (CurrentMode == CurveMode::Bezier)
		{
			generated_vertices = CurveMath::GenerateBezierCurve(*ControlPoints, 100);
		}
		else
		{
			generated_vertices = CurveMath::GenerateBSplineSubdivision(*ControlPoints, SubdivisionLevel);
		}
		CurveVertexCount = generated_vertices.size();
		CurveGpu->setVerts(generated_vertices);
	}


public:
	CurveEditorCallBack() {}

	CurveEditorCallBack(std::vector<glm::vec3>* InControlPoints, GPU_Geometry* InPointGpu, GPU_Geometry* InLineGpu, GPU_Geometry* InCurveGpu)
		: ControlPoints(InControlPoints), PointGpu(InPointGpu), LineGpu(InLineGpu), CurveGpu(InCurveGpu),
		WindowWidth(800), WindowHeight(800), SelectedPointIndex(-1), IsDragging(false),
		CurrentMode(CurveMode::Bezier), SubdivisionLevel(1), CurveVertexCount(0)
	{
		UpdateBuffers();
	}
	int GetCurveVertexCount() const
	{
		return CurveVertexCount;
	}

	virtual void keyCallback(int key, int scancode, int action, int mods) override {
		//Log::info("KeyCallback: key={}, action={}", key, action);
		if (action == GLFW_PRESS)
		{
			// bezier (B)
			if (key == GLFW_KEY_B)
			{
				CurrentMode = CurveMode::Bezier;
				Log::info("CurveMode: Bezier");
				UpdateBuffers();
			}
			// b-spline (S)
			else if (key == GLFW_KEY_S)
			{
				CurrentMode = CurveMode::BSpline;
				Log::info("Curve Mode: B-Spline (Level {})", SubdivisionLevel);
				UpdateBuffers();
			}
			// up arrow, increases subdivision level for b-spline
			else if (key == GLFW_KEY_UP)
			{
				SubdivisionLevel++;
				if (SubdivisionLevel > 5) SubdivisionLevel = 5; // Cap at 5 as per assignment requirements
				Log::info("Subdivision Level Increased to: {}", SubdivisionLevel);

				if (CurrentMode == CurveMode::BSpline)
				{
					UpdateBuffers();
				}
			}
			// down arrow, decreases subdivision level
			else if (key == GLFW_KEY_DOWN)
			{
				SubdivisionLevel--;
				if (SubdivisionLevel < 0) SubdivisionLevel = 0;
				Log::info("Subdivision Level Decreased to: {}", SubdivisionLevel);

				if (CurrentMode == CurveMode::BSpline)
				{
					UpdateBuffers();
				}
			}
		}
	}

	virtual void mouseButtonCallback(int button, int action, int mods) override {
		//Log::info("MouseButtonCallback: button={}, action={}", button, action);

		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if (action == GLFW_PRESS)
			{
				
				float MinDist = 0.1f; // find the closest point
				int ClosestIndex = -1;

				for (size_t i = 0; i < ControlPoints->size(); ++i) // iterate through all cntrl points
				{
					// get the distance from our cursor to the control point
					float Dist = glm::length((*ControlPoints)[i] - MousePosNdc);
					if (Dist < MinDist)
					{
						MinDist = Dist;
						ClosestIndex = static_cast<int>(i);
					}
				}

				if (ClosestIndex != -1)
				{
					SelectedPointIndex = ClosestIndex;
					IsDragging = true;
				}
				else
				{
					Log::info("Adding new control point at position: ({}, {})", MousePosNdc.x, MousePosNdc.y);
					ControlPoints->push_back(MousePosNdc);
					UpdateBuffers();
					Log::info("Control Points Size: {}", ControlPoints->size());

				}
			}
			else if (action == GLFW_RELEASE)
			{
				IsDragging = false;
				SelectedPointIndex = -1;
			}
		}

		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			if (action == GLFW_PRESS)
			{

				float MinDist = 0.1f; // find the closest point
				int ClosestIndex = -1;

				for (size_t i = 0; i < ControlPoints->size(); i++) // iterate through all cntrl points
				{
					// get the distance from our cursor to the control point
					float Dist = glm::length((*ControlPoints)[i] - MousePosNdc);
					if (Dist < MinDist)
					{
						MinDist = Dist;
						ClosestIndex = static_cast<int>(i);
					}
				}

				if (ClosestIndex != -1)
				{
					// erase the point from the control points vector
					// takes in a ptr
					// ptr + idx*(elem size)
					Log::info("Removing control point at position: ({}, {})", (*ControlPoints)[ClosestIndex].x, (*ControlPoints)[ClosestIndex].y);
					ControlPoints->erase(ControlPoints->begin() + ClosestIndex);
					UpdateBuffers();
				}
			}
		}


	}

	virtual void cursorPosCallback(double xpos, double ypos) override {
		
		// we have to use * 2.0f - 1.0f because open GL uses NDC coordinates meaning the very left is -1.f and the very right is 1.f
		float X_Ndc = static_cast<float>(xpos) / WindowWidth * 2.0f - 1.0f;
		float Y_Ndc = -(static_cast<float>(ypos) / WindowHeight * 2.0f - 1.0f);

		//Log::info("CursorPosCallback: xpos={}, ypos={}", X_Ndc, Y_Ndc);

		MousePosNdc = glm::vec3(X_Ndc, Y_Ndc, 0.f);

		// if we are dragging a point, update that points position
		if (IsDragging && SelectedPointIndex != -1)
		{
			(*ControlPoints)[SelectedPointIndex] = MousePosNdc;
			UpdateBuffers();
		}
	}

	virtual void scrollCallback(double xoffset, double yoffset) override {
		Log::info("ScrollCallback: xoffset={}, yoffset={}", xoffset, yoffset);
	}

	virtual void windowSizeCallback(int width, int height) override {
		Log::info("WindowSizeCallback: width={}, height={}", width, height);
		CallbackInterface::windowSizeCallback(width, height); // Important, calls glViewport(0, 0, width, height);
	}
};

// Can swap the callback instead of maintaining a state machine
/*
class TurnTable3DViewerCallBack : public CallbackInterface {

public:
	TurnTable3DViewerCallBack() {}

	virtual void keyCallback(int key, int scancode, int action, int mods) {}
	virtual void mouseButtonCallback(int button, int action, int mods) {}
	virtual void cursorPosCallback(double xpos, double ypos) {}
	virtual void scrollCallback(double xoffset, double yoffset) {}
	virtual void windowSizeCallback(int width, int height) {

		// The CallbackInterface::windowSizeCallback will call glViewport for us
		CallbackInterface::windowSizeCallback(width, height);
	}
private:

};
*/

class CurveEditorPanelRenderer : public PanelRendererInterface {
public:
	CurveEditorPanelRenderer()
		: inputText(""), buttonClickCount(0), sliderValue(0.0f),
		dragValue(0.0f), inputValue(0.0f), checkboxValue(false),
		comboSelection(0)
	{
		// Initialize options for the combo box
		options[0] = "Option 1";
		options[1] = "Option 2";
		options[2] = "Option 3";

		// Initialize color (white by default)
		colorValue[0] = 1.0f; // R
		colorValue[1] = 1.0f; // G
		colorValue[2] = 1.0f; // B
	}

	virtual void render() override {
		ImGui::Begin("Window name:");
		// Color selector
		ImGui::ColorEdit3("Select Background Color", colorValue); // RGB color selector
		ImGui::Text("Selected Color: R: %.3f, G: %.3f, B: %.3f", colorValue[0], colorValue[1], colorValue[2]);

		// Text input
		ImGui::InputText("Input Text", inputText, IM_ARRAYSIZE(inputText));

		// Display the input text
		ImGui::Text("You entered: %s", inputText);

		// Button
		if (ImGui::Button("Click Me")) {
			buttonClickCount++;
		}
		ImGui::Text("Button clicked %d times", buttonClickCount);

		// Scrollable block
		ImGui::TextWrapped("Scrollable Block:");
		ImGui::BeginChild("ScrollableChild", ImVec2(0, 100), true); // Create a scrollable child
		for (int i = 0; i < 20; i++) {
			ImGui::Text("Item %d", i);
		}
		ImGui::EndChild();

		// Float slider
		ImGui::SliderFloat("Float Slider", &sliderValue, 0.0f, 100.0f, "Slider Value: %.3f");

		// Float drag
		ImGui::DragFloat("Float Drag", &dragValue, 0.1f, 0.0f, 100.0f, "Drag Value: %.3f");

		// Float input
		ImGui::InputFloat("Float Input", &inputValue, 0.1f, 1.0f, "Input Value: %.3f");

		// Checkbox
		ImGui::Checkbox("Enable Feature", &checkboxValue);
		ImGui::Text("Feature Enabled: %s", checkboxValue ? "Yes" : "No");

		// Combo box
		ImGui::Combo("Select an Option", &comboSelection, options, IM_ARRAYSIZE(options));
		ImGui::Text("Selected: %s", options[comboSelection]);

		// Displaying current values
		ImGui::Text("Slider Value: %.3f", sliderValue);
		ImGui::Text("Drag Value: %.3f", dragValue);
		ImGui::Text("Input Value: %.3f", inputValue);
		ImGui::End();
	}

	glm::vec3 getColor() const {
		return glm::vec3(colorValue[0], colorValue[1], colorValue[2]);
	}

private:
	float colorValue[3];  // Array for RGB color values
	char inputText[256];  // Buffer for input text
	int buttonClickCount; // Count button clicks
	float sliderValue;    // Value for float slider
	float dragValue;      // Value for drag input
	float inputValue;     // Value for float input
	bool checkboxValue;   // Value for checkbox
	int comboSelection;   // Index of selected option in combo box
	const char* options[3]; // Options for the combo box
};




int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();//MUST call this first to set up environment (There is a terminate pair after the loop)
	
	{ // Extra scope to ensure all GL releated destructors are called (for example in Window class) before glfwTerminate
		
		constexpr int multisampleAmountHint = 32;
		Window window(800, 800, "CPSC 453 Assignment 3", multisampleAmountHint);
				
		GLDebug::enable(); // Enable OpenGL debug/error printouts to console.

		// Currently, this boilerplate may generate many OpenGL messages with
		// the ID 131185, e.g. "131185 -- Other: Buffer detailed info: Buffer
		// object 6 (bound to GL_ELEMENT_ARRAY_BUFFER_ARB, usage hint is 
		// GL_STREAM_DRAW) will use VIDEO memory as the source for buffer object
		// operations...."
		// Because this is not important information for us and floods our
		// console, we'll ignore it. If you are getting flooded with another
		// non-critical message on your device, you may add other IDs to the
		// exclusion here.
		GLDebug::configureIgnoreIds({131185});
	
		// Can swap the callback instead of maintaining a state machine
		//window.setCallbacks(turn_table_3D_viewer_callback);
		
		std::shared_ptr<AssetPath> assetPath = AssetPath::Instance();

		// SHADERS
		ShaderProgram shader_program_default(
			assetPath->Get("shaders/basic.vert"), 
			assetPath->Get("shaders/basic.frag")
		); // Render pipeline we will use (You can use more than one!)

		std::vector<glm::vec3> cp_positions_vector = {
			{-.5f, -.5f, 0.f},
			{ .5f, -.5f, 0.f},
			{ .5f,  .5f, 0.f},
			{-.5f,  .5f, 0.f}
		};
		glm::vec3 cp_point_colour	= { 1.f,0.f,0.f };
		glm::vec3 cp_line_colour	= { 0.f,1.f,0.f };

		CPU_Geometry cp_point_cpu;
		cp_point_cpu.verts	= cp_positions_vector;
		cp_point_cpu.cols	= std::vector<glm::vec3>(cp_point_cpu.verts.size(), cp_point_colour);
		GPU_Geometry cp_point_gpu;
		cp_point_gpu.setVerts(cp_point_cpu.verts);
		cp_point_gpu.setCols(cp_point_cpu.cols);

		CPU_Geometry cp_line_cpu;
		cp_line_cpu.verts	= cp_positions_vector; // We are using GL_LINE_STRIP (change this if you want to use GL_LINES)
		cp_line_cpu.cols	= std::vector<glm::vec3>(cp_point_cpu.verts.size(), cp_line_colour);
		GPU_Geometry cp_line_gpu;
		cp_line_gpu.setVerts(cp_line_cpu.verts);
		cp_line_gpu.setCols(cp_line_cpu.cols);

		std::vector<glm::vec3> bezier_verticies = CurveMath::GenerateBezierCurve(cp_positions_vector, 100);
		glm::vec3 beizer_line_col = { 0.0f, 0.0f, 1.0f };

		CPU_Geometry curve_cpu;
		curve_cpu.verts = bezier_verticies;
		curve_cpu.cols = std::vector<glm::vec3>(curve_cpu.verts.size(), beizer_line_col);
		GPU_Geometry curve_gpu;
		curve_gpu.setVerts(curve_cpu.verts);
		curve_gpu.setCols(curve_cpu.cols);


		// CALLBACKS
		auto curve_editor_callback = std::make_shared<CurveEditorCallBack>(&cp_positions_vector, &cp_point_gpu, &cp_line_gpu, &curve_gpu);
		//auto turn_table_3D_viewer_callback = std::make_shared<TurnTable3DViewerCallBack>();

		auto curve_editor_panel_renderer = std::make_shared<CurveEditorPanelRenderer>();

		// You can change which PanelRendererInterface is assigned to this
		// "curr_panel" pointer as one possible way of switching between ImGui
		// panels during your program.
		std::shared_ptr<PanelRendererInterface> curr_panel = curve_editor_panel_renderer;

		//Set callback to window
		window.setCallbacks(curve_editor_callback);

		while (!window.shouldClose()) {
			// Three functions that must be called each new frame and before
        	// other ImGui calls!
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			glfwPollEvents();
			glm::vec3 background_colour = curve_editor_panel_renderer->getColor();

			//------------------------------------------
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_FRAMEBUFFER_SRGB);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glClearColor(background_colour.r, background_colour.g, background_colour.b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//------------------------------------------
			
			// Use the default shader (can use different ones for different objects)
			shader_program_default.use();

			//Render control points
			cp_point_gpu.bind();
			glPointSize(15.f);
			glDrawArrays(GL_POINTS, 0, cp_positions_vector.size());

			//Render curve connecting control points
			cp_line_gpu.bind();
			//glLineWidth(10.f); //May do nothing (like it does on my computer): https://community.khronos.org/t/3-0-wide-lines-deprecated/55426
			glDrawArrays(GL_LINE_STRIP, 0, cp_positions_vector.size());
			

			//Render the curve
			curve_gpu.bind();
			glDrawArrays(GL_LINE_STRIP, 0, curve_editor_callback->GetCurveVertexCount());

			//------------------------------------------
			glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui
			curr_panel->render(); // Render the current ImGui widget(s).

			// Our ImGui::Render() call does not actually display ImGui items to
			// the screen yet. It just prepares and sets ImGui's draw data.
			ImGui::Render();
			// Now we actually render the set ImGui data to screen. 
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			//------------------------------------------
			window.swapBuffers(); //Swap the buffers while displaying the previous
		}
		
	}

	glfwTerminate(); // Clean up GLFW
	return 0;
}
