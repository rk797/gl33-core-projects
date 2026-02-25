#pragma once
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


class DebugLine
{
public:
	CPU_Geometry cgeom;
	GPU_Geometry ggeom;
	DebugLine(glm::vec3 start, glm::vec3 end)
	{
		cgeom.verts.push_back(start);
		cgeom.verts.push_back(end);

		cgeom.uvs.push_back(glm::vec2(0.5f, 0.5f));
		cgeom.uvs.push_back(glm::vec2(0.5f, 0.5f));

		ggeom.setVerts(cgeom.verts);
		ggeom.setUVs(cgeom.uvs);

		ggeom.setVerts(cgeom.verts);

	}

	void draw()
	{
		ggeom.bind();
		glDrawArrays(GL_LINES, 0, 2);
	}


};