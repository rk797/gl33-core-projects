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
#include "OrbitCamera.h"

enum class EditorMode
{
	Editor2D,
	Viewer3D
};

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
	GPU_Geometry* SurfaceGpu;
	GPU_Geometry* TensorGpu;

	int WindowWidth;
	int WindowHeight;
	glm::vec3 MousePosNdc{};
	double LastMouseX = 0.0;
	double LastMouseY = 0.0;
	int SelectedPointIndex = 0;
	bool IsDragging = false;

	std::vector<std::vector<glm::vec3>> TensorControlGrid;
public:
	CurveMode CurrentCurveMode;
	int SubdivisionLevel = 0;
	int CurveVertexCount = 0;
	int SurfaceVertexCount = 0;
	bool ShowSurfaceColors = true;

	int TensorVertexCount = 0;



	EditorMode CurrentAppMode;
	OrbitCamera Camera;
	bool ShowSurface = false;
	bool ShowTensor = false;

	void UpdateBuffers()
	{
		PointGpu->setVerts(*ControlPoints);
		std::vector<glm::vec3> point_colors(ControlPoints->size(), glm::vec3(1.0f, 0.0f, 0.0f));

		if (SelectedPointIndex != -1 && SelectedPointIndex < point_colors.size())
		{
			point_colors[SelectedPointIndex] = glm::vec3(1.0f, 1.0f, 0.0f); // yellow for selected points
		}

		PointGpu->setCols(point_colors);


		LineGpu->setVerts(*ControlPoints);
		std::vector<glm::vec3> line_colors(ControlPoints->size(), glm::vec3(0.0f, 1.0f, 0.0f));
		LineGpu->setCols(line_colors);

		std::vector<glm::vec3> generated_vertices;

		if (CurrentCurveMode == CurveMode::Bezier)
		{
			generated_vertices = CurveMath::GenerateBezierCurve(*ControlPoints, 100);
		}
		else
		{
			generated_vertices = CurveMath::GenerateBSplineSubdivision(*ControlPoints, SubdivisionLevel);
		}
		CurveVertexCount = generated_vertices.size();
		CurveGpu->setVerts(generated_vertices);

		SurfaceGeometry surface_geom = CurveMath::GenerateSurfaceOfRevolution(generated_vertices, 40, ShowSurfaceColors);
		SurfaceVertexCount = surface_geom.vertices.size();
		if (SurfaceVertexCount > 0)
		{
			SurfaceGpu->setVerts(surface_geom.vertices);
			SurfaceGpu->setCols(surface_geom.colors);
		}

		SurfaceGeometry tensor_geom = CurveMath::GenerateTensorProductSurface(TensorControlGrid, SubdivisionLevel);
		TensorVertexCount = tensor_geom.vertices.size();
		if (TensorVertexCount > 0)
		{
			TensorGpu->setVerts(tensor_geom.vertices);
			TensorGpu->setCols(tensor_geom.colors);
		}

		// camera must always be focused on the centroid of the control points or origin
		if (CurrentAppMode == EditorMode::Editor2D || (!ShowSurface && !ShowTensor))
		{
			Camera.target_centroid = CurveMath::CalculateCentroid(*ControlPoints);
		}
		else
		{
			Camera.target_centroid = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}

	CurveEditorCallBack() {}

	CurveEditorCallBack(std::vector<glm::vec3>* InControlPoints, GPU_Geometry* InPointGpu, GPU_Geometry* InLineGpu, GPU_Geometry* InCurveGpu, GPU_Geometry* InSurfaceGpu, GPU_Geometry* InTensorGpu)
		: ControlPoints(InControlPoints), PointGpu(InPointGpu), LineGpu(InLineGpu), CurveGpu(InCurveGpu), SurfaceGpu(InSurfaceGpu), TensorGpu(InTensorGpu),
		WindowWidth(800), WindowHeight(800), SelectedPointIndex(-1), IsDragging(false),
		CurrentCurveMode(CurveMode::Bezier), SubdivisionLevel(1), CurveVertexCount(0), SurfaceVertexCount(0), ShowSurfaceColors(true), CurrentAppMode(EditorMode::Editor2D)
	{
		TensorControlGrid = CurveMath::GenerateSampleGrid();
		UpdateBuffers();
	}
	int GetCurveVertexCount() const
	{
		return CurveVertexCount;
	}
	int GetSurfaceVertexCount() const 
	{ 
		return SurfaceVertexCount; 
	}
	int GetTensorVertexCount() const 
	{ 
		return TensorVertexCount; 
	}

	virtual void keyCallback(int key, int scancode, int action, int mods) override {
		//Log::info("KeyCallback: key={}, action={}", key, action);
		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_TAB)
			{
				if (CurrentAppMode == EditorMode::Editor2D)
				{
					CurrentAppMode = EditorMode::Viewer3D;
					Log::info("Mode: 3D Viewer");
				}
				else
				{
					CurrentAppMode = EditorMode::Editor2D;
					Log::info("Mode: 2D Editor");
				}
				UpdateBuffers();
			}
			if (key == GLFW_KEY_1)
			{
				ShowSurface = !ShowSurface;
				UpdateBuffers(); 
			}
			else if (key == GLFW_KEY_2)
			{
				ShowSurfaceColors = !ShowSurfaceColors;
				UpdateBuffers();
			}
			// bezier (B)
			else if (key == GLFW_KEY_3)
			{
				CurrentCurveMode = CurveMode::Bezier;
				Log::info("CurveMode: Bezier");
				UpdateBuffers();
			}
			// b-spline (S)
			else if (key == GLFW_KEY_4)
			{
				CurrentCurveMode = CurveMode::BSpline;
				Log::info("Curve Mode: B-Spline (Level {})", SubdivisionLevel);
				UpdateBuffers();
			}
			// up arrow, increases subdivision level for b-spline
			else if (key == GLFW_KEY_UP)
			{
				SubdivisionLevel++;
				if (SubdivisionLevel > 5) SubdivisionLevel = 5; // Cap at 5 as per assignment requirements
				Log::info("Subdivision Level Increased to: {}", SubdivisionLevel);

				if (CurrentCurveMode == CurveMode::BSpline)
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

				if (CurrentCurveMode == CurveMode::BSpline)
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
				if (CurrentAppMode == EditorMode::Editor2D)
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
						SelectedPointIndex = ControlPoints->size();
						Log::info("Adding new control point at position: ({}, {})", MousePosNdc.x, MousePosNdc.y);
						ControlPoints->push_back(MousePosNdc);
						UpdateBuffers();
						Log::info("Control Points Size: {}", ControlPoints->size());

					}
				}
				else if (CurrentAppMode == EditorMode::Viewer3D)
				{
					IsDragging = true;
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

	virtual void cursorPosCallback(double xpos, double ypos) override
	{
		// we have to use * 2.0f - 1.0f because open GL uses NDC coordinates meaning the very left is -1.f and the very right is 1.f
		float x_ndc = static_cast<float>(xpos) / WindowWidth * 2.0f - 1.0f;
		float y_ndc = -(static_cast<float>(ypos) / WindowHeight * 2.0f - 1.0f);
		MousePosNdc = glm::vec3(x_ndc, y_ndc, 0.0f);
		//Log::info("CursorPosCallback: xpos={}, ypos={}", X_Ndc, Y_Ndc);

		if (CurrentAppMode == EditorMode::Editor2D && IsDragging && SelectedPointIndex != -1)
		{
			(*ControlPoints)[SelectedPointIndex] = MousePosNdc;
			UpdateBuffers();
		}
		else if (CurrentAppMode == EditorMode::Viewer3D && IsDragging)
		{
			// the direction of rotation should be the opposite of the drag direction.
			float delta_x = static_cast<float>(xpos - LastMouseX);
			float delta_y = static_cast<float>(ypos - LastMouseY);

			Camera.phi -= delta_x * 0.01f;
			Camera.theta -= delta_y * 0.01f;

			// clamp theta between +90 and -90 degrees
			float max_theta = (glm::pi<float>() / 2.0f) - 0.01f;
			if (Camera.theta > max_theta) Camera.theta = max_theta;
			if (Camera.theta < -max_theta) Camera.theta = -max_theta;
		}

		LastMouseX = xpos;
		LastMouseY = ypos;
	}


	virtual void scrollCallback(double xoffset, double yoffset) override
	{
		if (CurrentAppMode == EditorMode::Viewer3D)
		{
			Camera.distance -= static_cast<float>(yoffset) * 0.5f;
			if (Camera.distance < 0.1f) Camera.distance = 0.1f; // Prevent zooming through the target
		}
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

	std::shared_ptr<CurveEditorCallBack> EditorCallback;

	CurveEditorPanelRenderer(std::shared_ptr<CurveEditorCallBack> callback)
		: EditorCallback(callback)
	{
		// default col
		colorValue[0] = 0.15f;
		colorValue[1] = 0.15f;
		colorValue[2] = 0.15f;
	}

	virtual void render() override {
		ImGui::Begin("Curve Editor Settings");

		ImGui::Text("Global Settings");
		ImGui::ColorEdit3("Background Color", colorValue);

		ImGui::Separator();

		ImGui::Text("Geometry Controls");
		// If the slider is moved, update the variable AND trigger UpdateBuffers
		if (ImGui::SliderInt("Subdivision Level", &EditorCallback->SubdivisionLevel, 0, 5))
		{
			EditorCallback->UpdateBuffers();
		}

		ImGui::Separator();

		ImGui::Text("3D Viewer Toggles");
		if (ImGui::Checkbox("Show Surface of Revolution", &EditorCallback->ShowSurface))
		{
			// Prevent showing both shapes at the exact same time
			if (EditorCallback->ShowSurface) EditorCallback->ShowTensor = false;
			EditorCallback->UpdateBuffers();
		}

		if (ImGui::Checkbox("Show Tensor Surface", &EditorCallback->ShowTensor))
		{
			if (EditorCallback->ShowTensor) EditorCallback->ShowSurface = false;
			EditorCallback->UpdateBuffers();
		}

		ImGui::End();
	}

	glm::vec3 getColor() const {
		return glm::vec3(colorValue[0], colorValue[1], colorValue[2]);
	}

private:
	float colorValue[3];
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

		GPU_Geometry surface_gpu;
		GPU_Geometry tensor_gpu;


		// CALLBACKS
		auto curve_editor_callback = std::make_shared<CurveEditorCallBack>(&cp_positions_vector, &cp_point_gpu, &cp_line_gpu, &curve_gpu, &surface_gpu, &tensor_gpu);
		//auto turn_table_3D_viewer_callback = std::make_shared<TurnTable3DViewerCallBack>();

		auto curve_editor_panel_renderer = std::make_shared<CurveEditorPanelRenderer>(curve_editor_callback);

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

			GLint view_loc = glGetUniformLocation((GLuint)shader_program_default, "view");
			GLint proj_loc = glGetUniformLocation((GLuint)shader_program_default, "projection");

			// here I generate the matrices based on the mode
			glm::mat4 view_matrix;
			glm::mat4 projection_matrix;

			if (curve_editor_callback->CurrentAppMode == EditorMode::Editor2D)
			{
				// use identity matrices in 2d mode (which apply no transformation)
				view_matrix = glm::mat4(1.0f);
				projection_matrix = glm::mat4(1.0f);
			}
			else
			{
				// In 3D mode, I use the Orbit Camera and a perspective projection
				view_matrix = curve_editor_callback->Camera.GetViewMatrix();
				projection_matrix = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);
			}

			// send matrices to the shader
			glUniformMatrix4fv(view_loc, 1, GL_FALSE, &view_matrix[0][0]);
			glUniformMatrix4fv(proj_loc, 1, GL_FALSE, &projection_matrix[0][0]);


			// switch back to solid mode for next frame

			if (curve_editor_callback->CurrentAppMode == EditorMode::Editor2D)
			{
				cp_point_gpu.bind();
				glPointSize(15.f);
				glDrawArrays(GL_POINTS, 0, cp_positions_vector.size());

				cp_line_gpu.bind();
				glDrawArrays(GL_LINE_STRIP, 0, cp_positions_vector.size());

				curve_gpu.bind();
				glDrawArrays(GL_LINE_STRIP, 0, curve_editor_callback->GetCurveVertexCount());
			}
			else
			{
				
				if (curve_editor_callback->ShowTensor && curve_editor_callback->GetTensorVertexCount() > 0)
				{
					// Draw the Tensor product surface in wireframe
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					tensor_gpu.bind();
					glDrawArrays(GL_TRIANGLES, 0, curve_editor_callback->GetTensorVertexCount());
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				else if (curve_editor_callback->ShowSurface && curve_editor_callback->GetSurfaceVertexCount() > 0)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

					surface_gpu.bind();
					glDrawArrays(GL_TRIANGLES, 0, curve_editor_callback->GetSurfaceVertexCount());

					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
				else
				{
					cp_point_gpu.bind();
					glPointSize(15.f);
					glDrawArrays(GL_POINTS, 0, cp_positions_vector.size());
					curve_gpu.bind();
					glDrawArrays(GL_LINE_STRIP, 0, curve_editor_callback->GetCurveVertexCount());

				}
			}

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
