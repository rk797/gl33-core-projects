#pragma once

#include "../Geometry.h"
#include "../usr/Transformation.h"


// An example class for Game Objects.
// You are encouraged to customize or replace this as you see fit.
class GameObject {
public:
	// The constructor deals with the texture.
	// Also sets default position, theta, scale, and transformationMatrix.
	GameObject(glm::ivec2 textureDims, float targetWidth, glm::vec3 initPosition = {0.f, 0.f, 0.f});

	void bind(); // Binds the GPU_Geometry.
	void updateGeometry();
	virtual void update(GLFWwindow* window, float deltaTime) {};
	
	glm::vec3 position;

protected:	
	float theta; // Object's rotation
	// Alternatively, you could represent rotation via a normalized heading vec:
	// glm::vec3 heading;
	float scale; // Or, alternatively, a glm::vec2 scale;
	glm::mat4 transformationMatrix;

	// For Part IV, you should not have the the following attributes, as you
	// must use SpriteQuad!
	CPU_Geometry cgeom;
	GPU_Geometry ggeom;

	std::vector<glm::vec3> origVerts;
};

// For full marks (Part IV), you'll need to make sure that the only
// CPU_Geometry/GPU_Geometry pair you render is the one in SpriteQuad, NOT new
// ones like what this creates! So you can choose to remove this namespace
// and/or method at that time.
namespace PrePartIVGeom {
	CPU_Geometry quadPrePartIV(float width, float height, glm::vec3 offset = {0.f, 0.f, 0.f});
}