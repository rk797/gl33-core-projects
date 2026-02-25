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
	LocalPlayer(glm::ivec2 TextureDims, float TargetWidth)
		: GameObject(TextureDims, TargetWidth)
	{

	}

	void update(GLFWwindow* window, float deltaTime) override;
	void processInput(GLFWwindow* window, float deltaTime);
};