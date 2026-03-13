#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Window.h"
#include "Panel.h"
#include "AssetPath.h"


class CurveMath
{
public:

	static glm::vec3 DeCasteljau(const std::vector<glm::vec3> control_points, float u)
	{
		// Pnext = (1-u) * Pcurrent[i] + u * Pcurrent[i+1]
		// u: position along the bezier curve, between 0 and 1

		int degree = control_points.size() - 1;
		int num_points = control_points.size();
		std::vector<glm::vec3> current_points = control_points;
		for (int i = 1; i < num_points; i++)
		{
			for (int j = 0; j < num_points - i; j++)
			{
				current_points[j] = (1.f - u) * current_points[j] + u * current_points[j + 1];
			}
		}
		return current_points[0];
	}


	static std::vector<glm::vec3> GenerateBezierCurve(const std::vector<glm::vec3> control_points, int segments)
	{

		std::vector<glm::vec3> curve_points;

		if (control_points.size() < 2)
		{
			return curve_points;
		}

		for (int i = 0; i <= segments; i++)
		{
			// normlaize segments between 0 and 1
			float t = static_cast<float>(i) / (segments);
			glm::vec3 point_on_curve = DeCasteljau(control_points, t);
			curve_points.push_back(point_on_curve);
		}


		return curve_points;
	}
};
