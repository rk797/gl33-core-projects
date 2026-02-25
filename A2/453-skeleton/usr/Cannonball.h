#pragma once

#include "../game/GameObject.h"
#include "../game/Game.h"

class Cannonball : public GameObject
{
	glm::vec3 forward;
public:

	Cannonball(glm::vec3 forward, glm::vec3 init_pos)
		: GameObject(_Core.cannonTexture->getDimensions(), 0.02f, init_pos),
		forward(forward)
	{

	}

	void update(GLFWwindow* window, float deltaTime) override
	{
		position += forward * Game::cannonball_movement_speed_ndc_per_sec * deltaTime; // Assuming a frame time of 16ms (60 FPS)
	}
};