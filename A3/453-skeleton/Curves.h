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
#include <glm/gtc/constants.hpp>
struct SurfaceGeometry
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
};

class CurveMath
{
public:

	static SurfaceGeometry GenerateSurfaceOfRevolution(const std::vector<glm::vec3>& curve_points, int slices, bool use_color)
	{
		SurfaceGeometry geom;
		if (curve_points.empty() || slices < 3) return geom;

		float angle_step = (2.0f * glm::pi<float>()) / static_cast<float>(slices);

		std::vector<std::vector<glm::vec3>> grid_points(slices + 1);
		std::vector<glm::vec3> slice_colors(slices + 1);

		for (int i = 0; i <= slices; ++i)
		{
			// every slice theta gets increased by a fixed step
			float theta = i * angle_step;

			if (use_color)
			{
				//red based on cos, blue based on sin, green constant
				float r = 0.5f * (glm::cos(theta) + 1.0f);
				float b = 0.5f * (glm::sin(theta) + 1.0f);
				float g = 1.0f - r;
				slice_colors[i] = glm::vec3(r, g, b);
			}
			else
			{
				// default colour
				slice_colors[i] = glm::vec3(0.5f, 0.5f, 0.5f);
			}
		

			for (const glm::vec3& pt : curve_points)
			{
				float new_x = pt.x * glm::cos(theta);
				float new_y = pt.y; // Y is the axis of revolution so it remains unchanged
				float new_z = pt.x * glm::sin(theta);

				grid_points[i].push_back(glm::vec3(new_x, new_y, new_z));
			}
		}


		// here i convert the grid poitns into triangles for rendering
		int num_curve_pts = curve_points.size();

		for (int i = 0; i < slices; ++i)
		{
			for (int j = 0; j < num_curve_pts - 1; ++j)
			{
				// i : longitude (along Y)
				// j : latitude (along the curve)
				glm::vec3 bottom_left = grid_points[i][j];
				glm::vec3 top_left = grid_points[i][j + 1];
				glm::vec3 bottom_right = grid_points[i + 1][j];
				glm::vec3 top_right = grid_points[i + 1][j + 1];

				glm::vec3 color_left = slice_colors[i];
				glm::vec3 color_right = slice_colors[i + 1];

				// triangle 1, bl, tl, br
				geom.vertices.push_back(bottom_left);
				geom.colors.push_back(color_left);
				geom.vertices.push_back(top_left);
				geom.colors.push_back(color_left);
				geom.vertices.push_back(bottom_right);
				geom.colors.push_back(color_right);

				// triangle 2, br, tl, tr
				geom.vertices.push_back(bottom_right);
				geom.colors.push_back(color_right);
				geom.vertices.push_back(top_left);
				geom.colors.push_back(color_left);
				geom.vertices.push_back(top_right);
				geom.colors.push_back(color_right);
			}
		}

		return geom;
	}

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

	static std::vector<glm::vec3> GenerateBSplineSubdivision(const std::vector<glm::vec3>& control_points, int sub_lvl)
	{
		std::vector<glm::vec3> current_points = control_points;

		for (int i = 0; i < sub_lvl; i++)
		{
			// atleast 3 points are required to calcualte cubic b-spline
			if (current_points.size() < 3)
			{
				break;
			}

			std::vector<glm::vec3> new_points;
			new_points.push_back(current_points[0]);
			for (int j = 0; j < current_points.size() - 1; j++)
			{
				// calculate the midpoint between the two adjacent points
				glm::vec3 pnt_edge = 0.5f * current_points[j] + 0.5f * current_points[j + 1];

				if (j > 0)
				{
					// this calculation is for the vertex
					glm::vec3 pnt_vertex = 0.125f * current_points[j - 1] + 0.75f * current_points[j] + 0.125f * current_points[j + 1];
					new_points.push_back(pnt_vertex);
				}

				new_points.push_back(pnt_edge);
			}
			new_points.push_back(current_points[current_points.size() - 1]);
			current_points = new_points;
		}
		return current_points;
	}

	static glm::vec3 CalculateCentroid(const std::vector<glm::vec3>& points)
	{
		if (points.empty())
		{
			return glm::vec3(0.0f, 0.0f, 0.0f);
		}

		glm::vec3 sum(0.0f, 0.0f, 0.0f);
		for (const glm::vec3& p : points)
		{
			sum += p;
		}

		return sum / static_cast<float>(points.size());
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


	static std::vector<std::vector<glm::vec3>> GenerateSampleGrid()
	{
		// creates an 8x8 grid of control points in the XZ plane with varitation in the Y direction
		std::vector<std::vector<glm::vec3>> grid;
		int num_rows = 8;
		int num_cols = 12;

		for (int i = 0; i < num_rows; i++)
		{
			std::vector<glm::vec3> row;

			// take max idx of row and divide by 2 to get the center
			// multiplying by 0.5 gives the spacing between the points
			float z = (i - (num_rows - 1) / 2.0f) * 0.5f;

			for (int j = 0; j < num_cols; j++)
			{
				float x = (j - (num_cols - 1) / 2.0f) * 0.5f;

				// wavy pattern
				float y = glm::sin(x) * glm::cos(z) * 0.9f;

				row.push_back(glm::vec3(x, y, z));
			}
			grid.push_back(row);
		}
		return grid;
	}
private:
	// in place transpose function
	static inline std::vector<std::vector<glm::vec3>> TransposeGrid(std::vector<std::vector<glm::vec3>>& matrix)
	{
		if (matrix.empty()) return {};

		size_t rows = matrix.size();
		size_t cols = matrix[0].size();

		std::vector<std::vector<glm::vec3>> transposed(cols, std::vector<glm::vec3>(rows));

		for (size_t i = 0; i < rows; i++) 
		{
			for (size_t j = 0; j < cols; j++) 
			{
				transposed[j][i] = matrix[i][j];
			}
		}

		return transposed;
	}
public:

	static SurfaceGeometry GenerateTensorProductSurface(const std::vector<std::vector<glm::vec3>>& control_grid, int sub_lvl)
	{
		std::vector<std::vector<glm::vec3>> current_grid = control_grid;

		for (int lvl = 0; lvl < sub_lvl; ++lvl)
		{
			// sub divide alogn u direction using chaikins algorithm
			std::vector<std::vector<glm::vec3>> u_subdivided;
			for (const auto& row : current_grid)
			{
				std::vector<glm::vec3> new_row;

				// early exit if the row is small
				// no point in sub-dividing if the row is small
				if (row.size() < 2) 
				{
					u_subdivided.push_back(row); 
					continue; 
				}

				new_row.push_back(row.front()); // don't subdivide the start
				for (size_t i = 0; i < row.size() - 1; ++i)
				{
					// 2nd degree chaikin's algorithm
					new_row.push_back(0.75f * row[i] + 0.25f * row[i + 1]);
					new_row.push_back(0.25f * row[i] + 0.75f * row[i + 1]);
				}
				new_row.push_back(row.back()); // end should stay exactly where it was
				u_subdivided.push_back(new_row);
			}

			// sub divide along v direction using chaikins again
			if (u_subdivided.empty()) break;
			int num_rows = u_subdivided.size();
			int num_cols = u_subdivided[0].size();


			// this wil transpose the grid so that I can iterate over the colums easily using a for loop

			auto transposed_grid = TransposeGrid(u_subdivided);

			std::vector<std::vector<glm::vec3>> subdivided_cols;
			for (const auto& col : transposed_grid)
			{
				std::vector<glm::vec3> new_col;
				if (col.size() < 3) { subdivided_cols.push_back(col); continue; }

				new_col.push_back(col[0]); // start should stay the same
				for (size_t i = 0; i < col.size() - 1; ++i)
				{
					glm::vec3 pnt_edge = 0.5f * col[i] + 0.5f * col[i + 1];
					if (i > 0)
					{
						// 3rd degree chaikins algorithm as defined in the notes
						glm::vec3 pnt_vertex = 0.125f * col[i - 1] + 0.75f * col[i] + 0.125f * col[i + 1];
						new_col.push_back(pnt_vertex);
					}
					new_col.push_back(pnt_edge);
				}
				new_col.push_back(col.back()); // end should be the same
				subdivided_cols.push_back(new_col);
			}

			// transpose the grid back to its original orientation
			int new_num_cols = subdivided_cols.size();
			int new_num_rows = subdivided_cols[0].size();

			auto next_grid = TransposeGrid(subdivided_cols);

			current_grid = next_grid;
		}



		// conver the subdivided grid into triangles for rendering
		SurfaceGeometry geom;
		if (current_grid.empty()) return geom;
		int rows = current_grid.size();
		int cols = current_grid[0].size();

		for (int r = 0; r < rows - 1; r++)
		{
			for (int c = 0; c < cols - 1; c++)
			{
				glm::vec3 bottom_left = current_grid[r][c];
				glm::vec3 top_left = current_grid[r + 1][c];
				glm::vec3 bottom_right = current_grid[r][c + 1];
				glm::vec3 top_right = current_grid[r + 1][c + 1];

				glm::vec3 surface_color(0.5f, 0.5f, 0.5f); 

				geom.vertices.push_back(bottom_left); geom.colors.push_back(surface_color);
				geom.vertices.push_back(top_left); geom.colors.push_back(surface_color);
				geom.vertices.push_back(bottom_right); geom.colors.push_back(surface_color);

				geom.vertices.push_back(bottom_right); geom.colors.push_back(surface_color);
				geom.vertices.push_back(top_left); geom.colors.push_back(surface_color);
				geom.vertices.push_back(top_right); geom.colors.push_back(surface_color);
			}
		}

		return geom;
	}
};
