#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"
#include "AssetPath.h"
#include "game/Game.h"
#include "game/GameObject.h"
#include "time/FrameTime.h"
#include "usr/LocalPlayer.h"


// EXAMPLE CALLBACK
class SampleCallbacksA2 : public CallbackInterface {

public:
	SampleCallbacksA2(Window& window, ShaderProgram& shader) : window(window), shader(shader) {}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		// Now that we're using both ImGui and GLFW, and both can respond to
		// keyboard input, we need to make sure that the right events are going
		// to the right places.
		ImGui_ImplGlfw_KeyCallback(window.getGLFWwindow(), key, scancode, action, mods);
		// We check here whether ImGui is the one that should currently be
		// receiving keyboard input; e.g., if we were typing into a text field
		// in ImGui.
		if (!ImGui::GetIO().WantCaptureKeyboard) {
			// If ImGui isn't capturing the keyboard, we perform our regular
			// logic here.
			if (key == GLFW_KEY_R && action == GLFW_PRESS) {
				shader.recompile();
			}
		}
	}

	// Other callbacks we implemented that you may use
	//virtual void mouseButtonCallback(int button, int action, int mods) {}
	//virtual void cursorPosCallback(double xpos, double ypos) {}
	//virtual void scrollCallback(double xoffset, double yoffset) {}
	//virtual void windowSizeCallback(int width, int height) { CallbackInterface::windowSizeCallback(width, height);/*Should be called*/ }

private:
	Window& window; // Needed for an ImGui call.
	ShaderProgram& shader;
};

// END EXAMPLE


int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();//MUST call this first to set up environment (There is a terminate pair after the loop)
	
	{ // Extra scope to ensure all GL releated destructors are called (for example in Window class) before glfwTerminate
		
		// This is new for Assignment 2. This tells the window creation how many
		// samples to use for something called multisampling, which will make
		// the textures look a bit better. Read more at:
		// https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
		constexpr int multisampleAmountHint = 32;
		
		Window window(800, 800, "CPSC 453 Assignment 2", multisampleAmountHint);
				
		GLDebug::enable(); // Enable OpenGL debug/error printouts to console.

		// Since the above "setting" of GLFW_SAMPLES is only treated as a "hint"
		// by GLFW/OpenGL, this log statement tells us how many are *actually*
		// used. E.g., one one system, GL_SAMPLES might be a max of 16 despite
		// the hint of 32.
		int samples = 0;
		glGetIntegerv(GL_SAMPLES, &samples);
		Log::info("MSAA Samples: {0}", samples);
		
		// Our background (sorry, *ocean*) colour. Feel free to change this!
		glClearColor(0.2f, 0.6f, 0.8f, 1.0f);
		
		std::shared_ptr<AssetPath> assetPath = AssetPath::Instance();

		// SHADERS
		ShaderProgram shader(
			assetPath->Get("shaders/basic.vert"), 
			assetPath->Get("shaders/basic.frag")
		); // Render pipeline we will use (You can use more than one!)


		// CALLBACKS
		std::shared_ptr<SampleCallbacksA2> callback_ptr = std::make_shared<SampleCallbacksA2>(window, shader); // Class To capture input events
		
		
		window.setupImGui();		
		window.setCallbacks(callback_ptr); // Can also update callbacks to new ones as needed (create more than one instance)

		// This boilerplate's three texture parameters are the filepath, the
		// interpolation mode, and whether the source file is sRGB encoded.
		// Usually, one wants linear interpolation (GL_LINEAR) for the
		// interpolation mode so that textures look less pixelated. However,
		// because our game USES low-res pixel art, GL_NEAREST interpolation
		// (i.e., no smoothing) looks better. Also, most textures where colour
		// represents colour will be be made in sRGB mode by artists and so are
		// sRGB encoded, so we pass in "true" here. Exceptions that you might
		// see in a later assignment include ones where colour encodes a 3D
		// vector in space, like a face normal; in this case, the texture is
		// probably linear encoded and we'd pass in "false".
		// ---
		// Note: While it might be "best practice" in some ways to try and use
		// a unique_ptr in Modern C++, you are free to make this a shared_ptr
		// instead for convenience.		
    	auto playerTexture = std::make_unique<Texture>(
			assetPath->Get("textures/pirate_pack/sprites/ships/ship (6).png"),
			GL_NEAREST, true
		);
		auto enemyTexture = std::make_unique<Texture>(
			assetPath->Get("textures/pirate_pack/sprites/ships/ship (2).png"),
			GL_NEAREST, true
		);



		LocalPlayer localPlayer(playerTexture->getDimensions(), 0.2);
		GameObject enemy1(enemyTexture->getDimensions(), 0.2, glm::vec3(0.5f, 0.5f, 0.0f));


		// If the time value between frames is too small, deltaTime might become
		// small enough that our floating-point math operations become unstable.
		// So we cap the framerate to prevent this.
		constexpr int maxFPS = 200;
		FrameTime timeManager(maxFPS);
		
		Game gameManager;
		// RENDER LOOP
		while (!window.shouldClose()) {

			timeManager.startNewFrame();
			// You should use deltaTime to control movement!
			float deltaTime = timeManager.getDeltaTime();
			
			glfwPollEvents(); // Propagate events to the callback class


			glEnable(GL_FRAMEBUFFER_SRGB);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shader.use(); // Use "this" shader to render
			// This tells OpenGL that our fragment shader should sample pixels from
			// this texture.
			playerTexture->bind();
			localPlayer.processInput(window.getGLFWwindow(), deltaTime);
			localPlayer.updateGeometry();
			localPlayer.bind();

			
			glDrawArrays(GL_TRIANGLES, 0, 6);

			enemyTexture->bind();
			enemy1.bind();
			glDrawArrays(GL_TRIANGLES, 0, 6); // Render Triangle primatives, starting at index 0 (first) with a total of 3 elements (in this case 1 triangle)
			
			// https://www.viewsonic.com/library/creative-work/srgb-vs-adobe-rgb-which-one-to-use/
			glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui (if used)

			
			gameManager.RenderImGui();

			window.swapBuffers(); //Swap the buffers while displaying the previous
		}
		
	}

	glfwTerminate(); // Clean up GLFW
	return 0;
}
