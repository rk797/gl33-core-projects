#include "SpriteQuad.h"

SpriteQuad::SpriteQuad() {
	static const std::vector<glm::vec3> pos = {
		// Triangle 1
		{-1.f, -1.f, 0.f},
		{ 1.f, -1.f, 0.f},
		{ 1.f,  1.f, 0.f},
		// Triangle 2
		{-1.f, -1.f, 0.f},
		{ 1.f,  1.f, 0.f},
		{-1.f,  1.f, 0.f},
	};
	static const std::vector<glm::vec2> uv = {
		// Triangle 1
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
		// Triangle 2
		{0.f, 0.f},
		{1.f, 1.f},
		{0.f, 1.f},
	};

	setVerts(pos);
	setUVs(uv);
}

void SpriteQuad::bind() { GPU_Geometry::bind(); }
