#pragma once

#include <string>

#include "core/gpu/mesh.h"
#include "utils/glm_utils.h"


namespace object3D
{

	// Create square with given bottom left corner, length and color
	Mesh* Box(const std::string& name, glm::vec3 leftBottomCorner, float length, float width, float height);
	Mesh* CircleWithEdge(const std::string& name, float cx, float height, float cz, float radius, glm::vec3 color);
}
