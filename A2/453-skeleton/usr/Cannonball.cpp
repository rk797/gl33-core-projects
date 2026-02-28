#include "Cannonball.h"
#include "LocalPlayer.h"
#include "../game/Game.h"


extern LocalPlayer* pLocalPlayer;


Cannonball::Cannonball()
	:GameObject(_Core.cannonTexture->getDimensions(), 0.03f, pLocalPlayer->position)
{
	creation_time = std::chrono::steady_clock::now();
	forwardDir = pLocalPlayer->orthogDir;
}

Cannonball::Cannonball(glm::vec3 start_pos, glm::vec3 direction)
	:GameObject(_Core.cannonTexture->getDimensions(), 0.03f, start_pos)
{
	creation_time = std::chrono::steady_clock::now();
	forwardDir = direction;
}

void Cannonball::update(GLFWwindow* window, float deltaTime)
{
	position += forwardDir * Game::cannonball_movement_speed_ndc_per_sec * deltaTime;
}

bool Cannonball::checkExpired()
{
	auto now = std::chrono::steady_clock::now();
	float diff = std::chrono::duration<float>(now - creation_time).count();
	return (diff >= Game::cannon_reload_time_sec) || intersects_with_object;
}