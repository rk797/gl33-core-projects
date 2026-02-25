#include "Globals.h"
// If you want to do a rotation around a certain point you will need to do 2 different transformations.
// Rotate first and then translate
// 
class Transformation
{
public:
	Transformation() = default;

	static glm::mat3 Scale2D(float sx, float sy);
	static glm::mat3 Rotate2D(float radians);
	static glm::mat3 Translate2D(float tx, float ty);
	static void ApplyTransformation(CPU_Geometry& _Geom, std::vector<glm::vec3>& _OrigVerts, const glm::mat3& _Transform);

};