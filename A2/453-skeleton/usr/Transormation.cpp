#include "Transformation.h"




glm::mat3 Transformation::Scale2D(float sx, float sy)
{
	return glm::mat3(
		sx, 0.f, 0.f,
		0.f, sy, 0.f,
		0.f, 0.f, 1.f
	);

}

glm::mat3 Transformation::Rotate2D(float radians)
{
	float cosTheta = cos(radians);
	float sinTheta = sin(radians);
	return glm::mat3(
		cosTheta, -sinTheta, 0.f,
		sinTheta, cosTheta, 0.f,
		0.f, 0.f, 1.f
	);

}

glm::mat3 Transformation::Translate2D(float tx, float ty)
{
	return glm::mat3(
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		tx, ty, 1.f
	);
}


void Transformation::ApplyTransformation(CPU_Geometry& _Geom, std::vector<glm::vec3>& _OrigVerts, const glm::mat3& _Transform)
{
	for (size_t i = 0; i < _OrigVerts.size(); ++i)
	{
		glm::vec3 homoVert = glm::vec3(glm::vec2(_OrigVerts[i]), 1.f);
		glm::vec3 transformedHomoVert = _Transform * homoVert;
		_Geom.verts[i] = glm::vec3(transformedHomoVert.x, transformedHomoVert.y, 0.f);
	}
}