#pragma once
#include "../game/Game.h"
#include "../game/GameObject.h"
#include "Cannonball.h"
#include "Line.h"


class LocalPlayer : public GameObject
{
private:
	std::chrono::time_point<std::chrono::steady_clock> last_cannonball_time;
	std::vector<Cannonball> cannonballs;

public:
	glm::vec3 forwardDir;
	glm::vec3 orthogDir;
	LocalPlayer()
		: GameObject(_Core.playerTexture->getDimensions(), 0.2)
	{

	}

	void update(GLFWwindow* window, float deltaTime) override;
	void processInput(GLFWwindow* window, float deltaTime);
};