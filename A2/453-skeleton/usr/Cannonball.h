#pragma once

#include "../game/GameObject.h"
#include "Line.h"

class Cannonball : public GameObject
{
	glm::vec3 forwardDir;
	std::chrono::steady_clock::time_point creation_time;
public:
	bool intersects_with_object = false;


	// by default the localplayers pos and orthog will be used
	Cannonball();
	Cannonball(glm::vec3 start_pos, glm::vec3 direction);

	void update(GLFWwindow* window, float deltaTime) override;

	bool checkIntersection(glm::vec3 target)
	{
		glm::vec3 diff = target - position;
		float dist = glm::length(diff);

		//DebugLine line1(position, target);
		//line1.draw();
		float tolerance = 0.05;
		if (dist < tolerance)
		{
			// triggers the removal on the next frame's remove_if check
			intersects_with_object = true;
			return true;
		}
		return false;
	}
	bool checkExpired();
};