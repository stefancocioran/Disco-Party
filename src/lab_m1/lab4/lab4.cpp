#include "lab_m1/lab4/lab4.h"

#include <vector>
#include <string>
#include <iostream>

#include "lab_m1/lab4/transform3D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab4::Lab4()
{
}


Lab4::~Lab4()
{
}


void Lab4::Init()
{
	polygonMode = GL_FILL;

	Mesh* mesh = new Mesh("box");
	mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
	meshes[mesh->GetMeshID()] = mesh;

	// Initialize tx, ty and tz (the translation steps)
	translateX = 0;
	translateY = 0;
	translateZ = 0;


	// Initialize sx, sy and sz (the scale factors)
	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;

	// Initialize angular steps
	angularStepOX = 0;
	angularStepOY = 0;
	angularStepOZ = 0;


	// Bonus
	translate = 0;
	rotate = 0;
	sunPosX = 0;
	sunPosY = 4.f;
	sunPosZ = -3.f;
}


void Lab4::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Sets the screen area where to draw
	glm::ivec2 resolution = window->GetResolution();
	glViewport(0, 0, resolution.x, resolution.y);
}


void Lab4::Update(float deltaTimeSeconds)
{
	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(-2.5f, 0.5f, -1.5f);
	modelMatrix *= transform3D::Translate(translateX, translateY, translateZ);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(0.0f, 0.5f, -1.5f);
	modelMatrix *= transform3D::Scale(scaleX, scaleY, scaleZ);
	RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

	modelMatrix = glm::mat4(1);
	modelMatrix *= transform3D::Translate(2.5f, 0.5f, -1.5f);
	modelMatrix *= transform3D::RotateOX(angularStepOX);
	modelMatrix *= transform3D::RotateOY(angularStepOY);
	modelMatrix *= transform3D::RotateOZ(angularStepOZ);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

	// BONUS - Bounce box
	{
		int bounceFreq = 1.1f;
		int slowTime = 2.f;
		int speed = 1.5f;
		translate += deltaTimeSeconds;
		modelMatrix = glm::mat4(1);
		modelMatrix *= transform3D::Translate(4.5f, 0.5f, -1.5f);
		modelMatrix *= transform3D::Translate(translate * speed / slowTime, abs(sin(bounceFreq * Engine::GetElapsedTime() / slowTime)), 0);
		modelMatrix *= transform3D::RotateOZ(glm::pi<float>() + bounceFreq * Engine::GetElapsedTime() / slowTime);
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}


	// BONUS - Solar System
	{
		glm::mat4 sun = glm::mat4(1);
		glm::mat4 earth = glm::mat4(1);
		glm::mat4 moon = glm::mat4(1);
		rotate += deltaTimeSeconds;

		// Sun
		sun *= transform3D::Translate(sunPosX + translate * 0.25f, sunPosY, sunPosZ);
		RenderMesh(meshes["box"], shaders["VertexNormal"], sun);

		// Earth orbits the Sun and rotates  around its axis 
		earth *= sun * transform3D::RotateOX(rotate * 0.5f) *
			transform3D::Translate(0, 1, 1) * transform3D::Scale(0.5f, 0.5f, 0.5f) * transform3D::RotateOZ(rotate * 1.2f);
		RenderMesh(meshes["box"], shaders["VertexNormal"], earth);

		// Moon orbits the Earth and rotates  around its axis
		moon *= earth * transform3D::RotateOX(rotate * 0.25f) *
			transform3D::Translate(0, 1.2f, 1.2f) * transform3D::Scale(0.5f, 0.5f, 0.5f) * transform3D::RotateOZ(rotate * 1.75f);
		RenderMesh(meshes["box"], shaders["VertexNormal"], moon);
	}
}


void Lab4::FrameEnd()
{
	DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab4::OnInputUpdate(float deltaTime, int mods)
{
	// TODO(student): Add transformation logic

	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		int speed = 1.5f;
		int angularSpeed = 2.5f;

		if (window->KeyHold(GLFW_KEY_W))
			translateZ -= deltaTime * speed;
		else if (window->KeyHold(GLFW_KEY_S))
			translateZ += deltaTime * speed;

		if (window->KeyHold(GLFW_KEY_A))
			translateX -= deltaTime * speed;
		else if (window->KeyHold(GLFW_KEY_D))
			translateX += deltaTime * speed;

		if (window->KeyHold(GLFW_KEY_R))
			translateY -= deltaTime * speed;
		else if (window->KeyHold(GLFW_KEY_F))
			translateY += deltaTime * speed;


		if (window->KeyHold(GLFW_KEY_1)) {
			scaleX -= deltaTime * speed;
			scaleY -= deltaTime * speed;
			scaleZ -= deltaTime * speed;
		}
		else if (window->KeyHold(GLFW_KEY_2)) {
			scaleX += deltaTime * speed;
			scaleY += deltaTime * speed;
			scaleZ += deltaTime * speed;
		}

		if (window->KeyHold(GLFW_KEY_3))
			angularStepOX -= deltaTime * angularSpeed;
		else if (window->KeyHold(GLFW_KEY_4))
			angularStepOX += deltaTime * angularSpeed;

		if (window->KeyHold(GLFW_KEY_5))
			angularStepOZ -= deltaTime * angularSpeed;
		else if (window->KeyHold(GLFW_KEY_6))
			angularStepOZ += deltaTime * angularSpeed;

		if (window->KeyHold(GLFW_KEY_7))
			angularStepOY -= deltaTime * angularSpeed;
		else if (window->KeyHold(GLFW_KEY_8))
			angularStepOY += deltaTime * angularSpeed;
	}
}


void Lab4::OnKeyPress(int key, int mods)
{
	// Add key press event
	if (key == GLFW_KEY_SPACE)
	{
		switch (polygonMode)
		{
		case GL_POINT:
			polygonMode = GL_FILL;
			break;
		case GL_LINE:
			polygonMode = GL_POINT;
			break;
		default:
			polygonMode = GL_LINE;
			break;
		}
	}
}


void Lab4::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Lab4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event
}


void Lab4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
}


void Lab4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Lab4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab4::OnWindowResize(int width, int height)
{
}
