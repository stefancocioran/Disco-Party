#include "object3D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"


Mesh* object3D::Box(
	const std::string& name,
	glm::vec3 leftBottomCorner,
	float length,
	float width,
	float height)
{
	glm::vec3 corner = glm::vec3(0, 0, 0);

	std::vector<VertexFormat> vertices =
	{
		VertexFormat(corner + glm::vec3(0, 0,  0)),
		VertexFormat(corner + glm::vec3(0, height,  0)),
		VertexFormat(corner + glm::vec3(length, 0,  0)),
		VertexFormat(corner + glm::vec3(length, height,  0)),

		VertexFormat(corner + glm::vec3(0, 0,  width)),
		VertexFormat(corner + glm::vec3(0, height,  width)),
		VertexFormat(corner + glm::vec3(length, 0,  width)),
		VertexFormat(corner + glm::vec3(length, height,  width))
	};

	std::vector<unsigned int> indices =
	{
		0, 2, 1,
		2, 3, 1,
		4, 7, 6,
		4, 5, 7,
		2, 6, 7,
		2, 7, 3,
		7, 2, 6,
		1, 5, 4,
		0, 1, 4,
		1, 3, 7,
		1, 7, 5,
		0, 2, 6,
		0, 6, 4
	};

	Mesh* box = new Mesh(name);

	box->InitFromData(vertices, indices);
	return box;
}


Mesh* object3D::CircleWithEdge(
	const std::string& name,
	float cx, float height, float cz,
	float radius,
	glm::vec3 color)
{
	Mesh* circle = new Mesh(name);

	int i;
	int triangleAmount = 30; //# of triangles used to draw circle
	std::vector<VertexFormat> vertices =
	{
		VertexFormat(glm::vec3(cx,  height,  cz), color)
	};
	std::vector<unsigned int> indices = { 0 };

	int count = 0;

	// Body
	for (i = 0; i <= triangleAmount; i++) {
		vertices.push_back(VertexFormat(glm::vec3(
			cx + (radius * cos(i * 2 * glm::pi<float>() / triangleAmount)),
			height,
			cz + (radius * sin(i * 2 * glm::pi<float>() / triangleAmount))), color));
		indices.push_back(i + 1);
	}

	count += triangleAmount;

	// Body edge
	indices.push_back(count);
	radius += 0.5f;

	for (i = count; i <= count + triangleAmount + 1; i++) {
		vertices.push_back(VertexFormat(glm::vec3(
			cx + (radius * cos(i * 2 * glm::pi<float>() / triangleAmount)),
			0,
			cz + (radius * sin(i * 2 * glm::pi<float>() / triangleAmount))), glm::vec3(0)));
		indices.push_back(i + 1);
	}

	circle->SetDrawMode(GL_TRIANGLE_FAN);
	circle->InitFromData(vertices, indices);
	return circle;
}
