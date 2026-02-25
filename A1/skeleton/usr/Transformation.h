#include "DEFS.h"
// If you want to do a rotation around a certain point you will need to do 2 different transformations.
// Rotate first and then translate
// 
class Transformation
{
	Transformation() = default;

	glm::mat3 Scale2D(float sx, float sy);
	glm::mat3 Rotate2D(float angleDegrees);
	glm::mat3 Translate2D(float tx, float ty);
	void ApplyTransformation(CPU_Geometry& _Geom, std::vector<glm::vec3>& _OrigVerts, const glm::mat3& _Transform)

};