#include <glm/gtc/matrix_transform.hpp>

class OrbitCamera
{
public:
	float distance;
	float theta;
	float phi;
	glm::vec3 target_centroid;

	OrbitCamera() : distance(3.0f), theta(0.5f), phi(0.5f), target_centroid(0.0f) {}

	// stores all the translation and rotation needed to transform the world from its global origin into the perspective of the camera
	glm::mat4 GetViewMatrix() const
	{
		// formulas for converting spherical coords (considering z axis is up)
		// x = rsin(theta)cos(phi)
		// y = rsin(theta)sin(phi)
		// z = rcos(theta)
		// p = rsin(theta)
		// consdering y axis is up
		// x = rcos(theta)sin(phi)
		// y = rsin(theta)
		// z = rcos(theta)cos(phi)

		// p = rsin(theta)
		// converts spherical coordinates to cartesian coordinates for the camera position
		float cam_x = target_centroid.x + distance * cos(theta) * sin(phi);
		float cam_y = target_centroid.y + distance * sin(theta);
		float cam_z = target_centroid.z + distance * cos(theta) * cos(phi);

		glm::vec3 camera_pos(cam_x, cam_y, cam_z);
		//in opengl y points upwards, so we set the up vector to be in the y direction
		glm::vec3 up_vector(0.0f, 1.0f, 0.0f); // tells the camera which direction is the sky

		// generate the view matrix looking at the centroid
		return glm::lookAt(camera_pos, target_centroid, up_vector);
	}
};