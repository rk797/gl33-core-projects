#pragma once
#include "../game/GameObject.h"



class EnemyShip : public GameObject
{
private:
	glm::vec3 forwardDir;
public:
	EnemyShip();
	EnemyShip(glm::vec3 pos, float ang2player);

	void update(GLFWwindow* window, float deltaTime) override;





};