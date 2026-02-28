#pragma once
#include "../game/GameObject.h"
#include "../usr/Cannonball.h"


class EnemyShip : public GameObject
{
private:
	glm::vec3 forwardDir;
	glm::vec3 orthogDirLeft;
	glm::vec3 orthogDirRight;
	float reloadTimer = 0.f;

	std::vector<Cannonball> cannonballs;
public:
	bool isDead = false;
	EnemyShip();
	EnemyShip(glm::vec3 pos, float ang2player);

	void update(GLFWwindow* window, float deltaTime) override;

	bool checkIntersectionWithLocalPlayer();





};