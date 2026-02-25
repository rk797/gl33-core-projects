#include "../game/GameObject.h"



class LocalPlayer : public GameObject
{
public:
	LocalPlayer(glm::ivec2 TextureDims, float TargetWidth)
		: GameObject(TextureDims, TargetWidth)
	{

	}


	void processInput(GLFWwindow* window, float deltaTime)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			theta += Game::ship_rotation_speed_rad_per_sec * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			theta -= Game::ship_rotation_speed_rad_per_sec * deltaTime;
		}

		// since the player textures start facing to the right, we can use the unit circle for this
		glm::vec3 forwardDir(cos(theta), -sin(theta), 0.0f); // unit vector (direction)

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			position += forwardDir * Game::ship_movement_speed_ndc_per_sec * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			position -= forwardDir * Game::ship_movement_speed_ndc_per_sec * deltaTime;
		}
	}
};