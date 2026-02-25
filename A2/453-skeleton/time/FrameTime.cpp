#include "FrameTime.h"
#include "GLFW/glfw3.h"

#include <thread>
#include <chrono>

FrameTime::FrameTime(int maxFPS) : minFrameDuration(1.f/maxFPS) {

}

void FrameTime::startNewFrame() {
	// Sleep to ensure the fps is capped.
	const float preSleepTime = glfwGetTime();
	const float sleepDuration = minFrameDuration - (preSleepTime - lastTime);
	if (sleepDuration > 0.f) {
		std::this_thread::sleep_for(std::chrono::duration<float>(sleepDuration));
	}

	// Calculate the deltaTime and update lastTime.
	const float currTime = glfwGetTime();
	deltaTime = currTime - lastTime;
	lastTime = currTime;
}

float FrameTime::getDeltaTime() {
	return deltaTime;
}
