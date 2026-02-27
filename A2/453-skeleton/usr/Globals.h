#pragma once
#include "memory.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "../Geometry.h"
#include "../GLDebug.h"
#include "../Log.h"
#include "../ShaderProgram.h"
#include "../Shader.h"
#include "../Window.h"
#include "../AssetPath.h"
#include "../Texture.h"
#include "../AssetPath.h"

#define PI 3.14159265358979323846


struct Triangle
{

	glm::vec3 v0;
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 col;


	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 col)
		: v0(v0), v1(v1), v2(v2), col(col)

	{

	}
};


struct Core
{
	std::unique_ptr<Texture> playerTexture;
	std::unique_ptr<Texture> enemyTexture;
	std::unique_ptr<Texture> cannonTexture;

	void Init()
	{
		playerTexture = std::make_unique<Texture>(
			AssetPath::Instance()->Get("textures/pirate_pack/sprites/ships/ship (6).png"),
			GL_NEAREST, true
		);
		enemyTexture = std::make_unique<Texture>(
			AssetPath::Instance()->Get("textures/pirate_pack/sprites/ships/ship (2).png"),
			GL_NEAREST, true
		);
		cannonTexture = std::make_unique<Texture>(
			AssetPath::Instance()->Get("textures/pirate_pack/sprites/ship_parts/cannonBall.png"),
			GL_NEAREST, true
		);
	}
};
inline Core _Core;
