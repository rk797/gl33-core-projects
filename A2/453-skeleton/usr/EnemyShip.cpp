#include "Globals.h"
#include "EnemyShip.h"
#include "LocalPlayer.h"
#include "../game/Game.h"

extern LocalPlayer* pLocalPlayer;
extern Game* pGameManager;


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

bool EnemyShip::checkIntersectionWithLocalPlayer()
{
	glm::vec3 diff = pLocalPlayer->position - position;
	float dist = glm::length(diff);

	//DebugLine line1(position, target);
	//line1.draw();
	float tolerance = 0.1;
	if (dist < tolerance)
	{
		// triggers the removal of this enemy
		return true;
	}
	return false;
}

void EnemyShip::update(GLFWwindow* window, float deltaTime)
{
	_Core.enemyTexture->bind();

	if (pGameManager->gameOver) return;

	float x = cos(theta);
	float y = -sin(theta);

	forwardDir = glm::vec3(x, y, 0.0f);
	position += forwardDir * Game::ship_movement_speed_ndc_per_sec * deltaTime;

	glm::mat3 rotMat1 = Transformation::Rotate2D(90.f * (PI / 180.f));
	glm::mat3 rotMat2 = Transformation::Rotate2D(-90.f * (PI / 180.f));

	// since the player textures start facing to the right, we can use the unit circle for this
	forwardDir = glm::vec3(cos(theta), -sin(theta), 0.0f); // unit vector (direction)
	orthogDirRight = rotMat1 * forwardDir;
	orthogDirLeft = rotMat2 * forwardDir;


	// Debug the vectors
	/*
	DebugLine orthogDebugLine1(position, position + orthogDirLeft * 0.2f);
	DebugLine orthogDebugLine2(position, position + orthogDirRight * 0.2f);
	orthogDebugLine1.draw();
	orthogDebugLine2.draw();
	*/

	for (auto& ball : cannonballs)
	{
		if (ball.checkIntersection(pLocalPlayer->position))
		{
			pGameManager->health--;
		}
	}

	cannonballs.erase(
		// moves all elements that do not match the condition to the front of the vector and returns a pointer to the end
		// after that we can safely erase everything from the end of the remove_if to the end of the vector
		std::remove_if(cannonballs.begin(), cannonballs.end(), [](auto& b)
			{
				return b.checkExpired();
			}),
		cannonballs.end()
	);


	float limit = 1.5f;
	// check if the ship is out of bounds
	if (position.x > limit || position.x < -limit || position.y > limit || position.y < -limit)
	{
		float dy = pLocalPlayer->position.y - position.y;
		float dx = pLocalPlayer->position.x - position.x;
		theta = -atan2(dy, dx);
	}

	reloadTimer += deltaTime;
	glm::vec3 toPlayer = pLocalPlayer->position - position;
	float distanceToPlayer = glm::length(toPlayer);

	if (distanceToPlayer > 0.0f)
	{
		glm::vec3 normToPlayer = toPlayer / distanceToPlayer;

		// 1.0 means they point in the exam same direction
		float dotLeft = glm::dot(normToPlayer, orthogDirLeft);
		float dotRight = glm::dot(normToPlayer, orthogDirRight);

		// Here I used 0.98 to create a narrow cone of vision
		float alignmentThreshold = 0.99f;

		if (dotLeft > alignmentThreshold && reloadTimer >= Game::cannon_reload_time_sec)
		{
			cannonballs.emplace_back(position, orthogDirLeft);
			reloadTimer = 0.0f;
		}
		else if (dotRight > alignmentThreshold && reloadTimer >= Game::cannon_reload_time_sec)
		{
			cannonballs.emplace_back(position, orthogDirRight);
			reloadTimer = 0.0f;
		}
	}

	for (auto& ball : cannonballs)
	{
		ball.update(window, deltaTime);
		//ball.updateGeometry();
		glm::mat3 ballMat = ball.getTransform();
		GLint currentProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
		GLint loc = glGetUniformLocation(currentProgram, "transformMatrix");
		glUniformMatrix3fv(loc, 1, GL_FALSE, &ballMat[0][0]);
		ball.bind();
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}