#pragma once

#include "../game/GameObject.h"
#include "../game/Game.h"


class Cannonball : public GameObject
{
	glm::vec3 forward;
	std::chrono::steady_clock::time_point creation_time;
	bool bIntersectsWithObject = false;
public:

	Cannonball();

	void update(GLFWwindow* window, float deltaTime) override
	{
		position += forward * Game::cannonball_movement_speed_ndc_per_sec * deltaTime; // Assuming a frame time of 16ms (60 FPS)
	}

	bool checkExpired()
	{
		auto now = std::chrono::steady_clock::now();
		float diff = std::chrono::duration<float>(now - creation_time).count();
		return (diff >= Game::cannon_reload_time_sec) || bIntersectsWithObject;
	}
};