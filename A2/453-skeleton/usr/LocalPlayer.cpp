#include "LocalPlayer.h"
void LocalPlayer::processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		theta += Game::ship_rotation_speed_rad_per_sec * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		theta -= Game::ship_rotation_speed_rad_per_sec * deltaTime;
	}
	float radians = 90.f * (PI / 180.f);
	glm::mat3 rotMat = Transformation::Rotate2D(radians);

	// since the player textures start facing to the right, we can use the unit circle for this
	forwardDir = glm::vec3(cos(theta), -sin(theta), 0.0f); // unit vector (direction)
	orthogDir = rotMat * forwardDir;

	// Debug the vectors
	/*
	DebugLine forwardDebugLine(position, position + forwardDir * 0.2f);
	DebugLine orthogDebugLine(position, position + orthogDir * 0.2f);
	forwardDebugLine.draw();
	orthogDebugLine.draw();
	*/

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		position += forwardDir * Game::ship_movement_speed_ndc_per_sec * deltaTime;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		position -= forwardDir * Game::ship_movement_speed_ndc_per_sec * deltaTime;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		auto now = std::chrono::steady_clock::now();
		float diff = std::chrono::duration<float>(now - last_cannonball_time).count();
		if (diff >= Game::cannon_reload_time_sec)
		{
			std::cout << "CANNONBALL CREATED" << std::endl;	
			last_cannonball_time = now;
			cannonballs.emplace_back(); // add a new cannonball in the orthoganal direction of the ship
		}
	}
}


void LocalPlayer::update(GLFWwindow* window, float deltaTime)
{
	// render all the cannon balls here (and remove them if they go off screen)
	processInput(window, deltaTime);

	_Core.cannonTexture->bind();
	cannonballs.erase(
		// moves all elements that do not match the condition to the front of the vector and returns a pointer to the end
		// after that we can safely erase everything from the end of the remove_if to the end of the vector
		std::remove_if(cannonballs.begin(), cannonballs.end(), [](auto& b)
			{
				return b.checkExpired();
			}),
		cannonballs.end()
	);

	for (auto& ball : cannonballs)
	{
		ball.update(window, deltaTime);
		ball.updateGeometry();
		ball.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}


	_Core.playerTexture->bind();


}