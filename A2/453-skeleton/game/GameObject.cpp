#include "GameObject.h"



GameObject::GameObject(glm::ivec2 textureDims, float targetWidth, glm::vec3 initPosition)
	: position(0.0f, 0.0f, 0.0f)
	, theta(0)
	, scale(1)
	, transformationMatrix(1.0f) // This constructor sets it as the identity matrix
{
	glm::vec2 objectDims = (targetWidth / textureDims.x) * glm::vec2(textureDims);
	cgeom = PrePartIVGeom::quadPrePartIV(objectDims.x, objectDims.y, initPosition);
	ggeom.setVerts(cgeom.verts);
	ggeom.setUVs(cgeom.uvs);

	origVerts = cgeom.verts;

}

void GameObject::bind() { ggeom.bind(); }

// For full marks (Part IV), you'll need to make sure that the only
// CPU_Geometry/GPU_Geometry pair you render is the one in SpriteQuad, NOT new
// ones like what this creates! So you can choose to remove this namespace
// and/or method at that time.
CPU_Geometry PrePartIVGeom::quadPrePartIV(float width, float height, glm::vec3 offset) {
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	CPU_Geometry retGeom;
	// vertices for the spaceship quad
	retGeom.verts.push_back(glm::vec3(-halfWidth, halfHeight, 0.f) + offset);
	retGeom.verts.push_back(glm::vec3(-halfWidth, -halfHeight, 0.f) + offset);
	retGeom.verts.push_back(glm::vec3(halfWidth, -halfHeight, 0.f) + offset);
	retGeom.verts.push_back(glm::vec3(-halfWidth, halfHeight, 0.f) + offset);
	retGeom.verts.push_back(glm::vec3(halfWidth, -halfHeight, 0.f) + offset);
	retGeom.verts.push_back(glm::vec3(halfWidth, halfHeight, 0.f) + offset);


	// texture coordinates
	retGeom.uvs.push_back(glm::vec2(0.f, 1.f));
	retGeom.uvs.push_back(glm::vec2(0.f, 0.f));
	retGeom.uvs.push_back(glm::vec2(1.f, 0.f));
	retGeom.uvs.push_back(glm::vec2(0.f, 1.f));
	retGeom.uvs.push_back(glm::vec2(1.f, 0.f));
	retGeom.uvs.push_back(glm::vec2(1.f, 1.f));
	return retGeom;
}

void GameObject::updateGeometry()
{
	glm::mat3 scaleMat = Transformation::Scale2D(scale, scale);
	glm::mat3 rotMat = Transformation::Rotate2D(theta);
	glm::mat3 transMat = Transformation::Translate2D(position.x, position.y);

	glm::mat3 finalTransform = transMat * rotMat * scaleMat;
	Transformation::ApplyTransformation(cgeom, origVerts, finalTransform);

	ggeom.setVerts(cgeom.verts);
}