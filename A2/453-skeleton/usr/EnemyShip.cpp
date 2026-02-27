#include "Globals.h"
#include "EnemyShip.h"
#include "LocalPlayer.h"

extern LocalPlayer* pLocalPlayer;


EnemyShip::EnemyShip()
	: GameObject(_Core.enemyTexture->getDimensions(), 0.2)
{

}
EnemyShip::EnemyShip(glm::vec3 pos, float ang2player)
	: GameObject(_Core.enemyTexture->getDimensions(), 0.2)
{
	position = pos;
	theta = -ang2player;
}

void EnemyShip::update(GLFWwindow* window, float deltaTime)
{
	_Core.enemyTexture->bind();
	forwardDir = glm::vec3(cos(theta), -sin(theta), 0.0f); // unit vector (direction)
	//orthogDir = rotMat * forwardDir;

	// Debug the vectors
	/*
	DebugLine forwardDebugLine(position, position + forwardDir * 0.2f);
	DebugLine orthogDebugLine(position, position + orthogDir * 0.2f);
	forwardDebugLine.draw();
	orthogDebugLine.draw();
	*/
	
	position += forwardDir * Game::ship_movement_speed_ndc_per_sec * deltaTime;

}