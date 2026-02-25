#pragma once

#include "../Geometry.h"

// Note that the inheritance is not public
class SpriteQuad : GPU_Geometry {
public:
	SpriteQuad();
	void bind();
};