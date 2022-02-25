#include "lab_m1/Tema3/Tema3.h"
#include "lab_m1/Tema3/object3D.h"
#include <windows.h> 
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema3::Tema3()
{
}


Tema3::~Tema3()
{
}


void Tema3::Init()
{

	phase = 0;

	// Light & material properties
	{
		materialShininess = 32;
		materialKd = 0.7;
		materialKs = 1;
	}

	// Camera
	{
		fov = 50.f;
		znear = 0.01f;
		zfar = 200.0f;

		left = -5.f;
		right = 5.f;
		bottom = -5.f;
		top = 5.f;

		camera = new implemented::GameCamera();
		camera->Set(glm::vec3(tilesCount / 2, 2, 12), glm::vec3(tilesCount / 2, 2, 5), glm::vec3(0, 1, 0));
	}

	// Load textures
	const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "textures");
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D(PATH_JOIN(sourceTextureDir, "disco.jpg").c_str(), GL_REPEAT);
		mapTextures["disco"] = texture;
		mapTextures["globe"] = CreateRandomTexture(16, 16);
	}


	// Load meshes
	{
		Mesh* mesh = new Mesh("quad");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("plane");
		mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* dancer = object3D::Box("dancer", glm::vec3(0), dancerSize, dancerSize, 0.75f);
		AddMeshToList(dancer);
	}

	{
		Mesh* tile = object3D::Box("tile", glm::vec3(0), 1, 1, 0);
		AddMeshToList(tile);
	}

	Mesh* cone = object3D::CircleWithEdge("cone", 0.5, roofHeight, 0.5, 1 / 2, glm::vec3(0));
	AddMeshToList(cone);

	CreateDiscoScene();

	// Load shaders
	{
		Shader* shader = new Shader("DiscoShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "DiscoVS.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "DiscoFS.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	{
		Shader* shader = new Shader("BasicShader");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "BasicVS.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "BasicFS.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, znear, zfar);
}

void Tema3::CreateDiscoScene() {
	// Generate Dancers
	srand(time(NULL));
	{
		for (int i = 0; i < dancersCount; i++) {
			dancer newDancer{
				RandomFloat(0.f, tilesCount - dancerSize),
				RandomFloat(0.f, tilesCount - dancerSize),
				glm::vec3(RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f),	RandomFloat(0.f, 1.f)),
				rand() % 2,
				rand() % 2,
				Engine::GetElapsedTime() };
			dancers.push_back(newDancer);
		}
	}

	// Generate tiles
	{
		int count = 0;
		for (int i = 0; i < tilesCount; i++) {
			for (int j = 0; j < tilesCount; j++) {
				tile newTile{
					i + tileSize / 2,
					j + tileSize / 2,
					glm::vec3(RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f),	RandomFloat(0.f, 1.f))
				};

				tilePos[count] = glm::vec3(newTile.cx, 0.2, newTile.cz);
				tileColor[count] = newTile.color;
				tileDir[count] = glm::vec3(0, -1, 0);

				tiles.push_back(newTile);
				count++;
			}
		}
	}

	// Generate cones
	{
		glm::vec3 color1 = glm::vec3(RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f));
		glm::vec3 color2 = glm::vec3(RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f));
		glm::vec3 color3 = glm::vec3(RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f));
		glm::vec3 color4 = glm::vec3(RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f), RandomFloat(0.f, 1.f));

		glm::vec3 pos1 = glm::vec3(1.5, roofHeight, 1.5);
		glm::vec3 pos2 = glm::vec3(6.5, roofHeight, 1.5);
		glm::vec3 pos3 = glm::vec3(1.5, roofHeight, 6.5);
		glm::vec3 pos4 = glm::vec3(6.5, roofHeight, 6.5);

		lightCones.push_back({ pos1, glm::vec3(0, -1, 0), color1 });
		lightCones.push_back({ pos2, glm::vec3(0, -1, 0), color2 });
		lightCones.push_back({ pos3, glm::vec3(0, -1, 0), color3 });
		lightCones.push_back({ pos4, glm::vec3(0, -1, 0), color4 });

		for (int i = 0; i < 4; i++) {
			coneColor[i] = lightCones.at(i).color;
			coneDir[i] = lightCones.at(i).lightDirection;
			conePos[i] = lightCones.at(i).lightPosition - glm::vec3(0, 0.01f, 0);
		}
	}
}

float Tema3::RandomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

Texture2D* Tema3::CreateRandomTexture(unsigned int width, unsigned int height)
{
	GLuint textureID = 0;
	unsigned int channels = 3;
	unsigned int size = width * height * channels;
	unsigned char* data = new unsigned char[size];

	// Generate random texture data
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			data[(i * width + j) * 3 + 0] = rand() % 255;
			data[(i * width + j) * 3 + 1] = rand() % 255;
			data[(i * width + j) * 3 + 2] = rand() % 255;
		}
	}

	// Generate and bind the new texture ID
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	if (GLEW_EXT_texture_filter_anisotropic) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	}
	// Set the texture parameters (MIN_FILTER, MAG_FILTER and WRAPPING MODE) using glTexParameteri
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	CheckOpenGLError();

	// Use glTexImage2D to set the texture data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// Generate texture mip-maps
	glGenerateMipmap(GL_TEXTURE_2D);

	CheckOpenGLError();

	// Save the texture into a wrapper Texture2D class for using easier later during rendering phase
	Texture2D* texture = new Texture2D();
	texture->Init(textureID, width, height, channels);

	SAFE_FREE_ARRAY(data);
	return texture;
}


void Tema3::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}


void Tema3::UpdateDancers(float deltaTimeSeconds) {
	// Dancers
	for (dancer& dancer : dancers) {
		if (Engine::GetElapsedTime() - dancer.lastChange < timeChangeDirection) {
			// Mentain direction
			if (dancer.dirX == 0) {
				if (dancer.translateX + deltaTimeSeconds * dancerSpeed < tilesCount - dancerSize) {
					dancer.translateX += deltaTimeSeconds * dancerSpeed;
				}
			}
			else {
				if (dancer.translateX - deltaTimeSeconds * dancerSpeed > 0) {
					dancer.translateX -= deltaTimeSeconds * dancerSpeed;
				}
			}

			if (dancer.dirZ == 1) {
				if (dancer.translateZ + deltaTimeSeconds * dancerSpeed < tilesCount - dancerSize) {
					dancer.translateZ += deltaTimeSeconds * dancerSpeed;
				}
			}
			else {
				if (dancer.translateZ - deltaTimeSeconds * dancerSpeed > 0) {
					dancer.translateZ -= deltaTimeSeconds * dancerSpeed;
				}
			}
		}
		else {
			// Change direction
			dancer.dirX = rand() % 2;
			dancer.dirZ = rand() % 2;
			dancer.lastChange = Engine::GetElapsedTime();
		}
	}
}

void Tema3::DrawRoom() {
	// Roof
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, roofHeight, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(4));
		modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 0, 1));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
		RenderSimpleMesh(meshes["quad"], shaders["DiscoShader"], modelMatrix, glm::vec3(0.5), mapTextures["globe"]);
	}

	// Walls
	{
		// Back
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(4, 2, 1));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 1, 0));
			RenderSimpleMesh(meshes["quad"], shaders["DiscoShader"], modelMatrix, glm::vec3(0.5), mapTextures["globe"]);
		}
		// Left
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, tilesCount));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(0, 1, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(4, 2, 1));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 1, 0));
			RenderSimpleMesh(meshes["quad"], shaders["DiscoShader"], modelMatrix, glm::vec3(0.5), mapTextures["globe"]);
		}

		// Right
		{
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(tilesCount, 0, 0));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(0, 1, 0));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(4, 2, 1));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 1, 0));
			RenderSimpleMesh(meshes["quad"], shaders["DiscoShader"], modelMatrix, glm::vec3(0.5), mapTextures["globe"]);
		}
	}
}

void Tema3::DrawCones() {
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDepthMask(GL_FALSE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (phase == 0 || phase == 3) {
		for (lightCone cone : lightCones) {
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, cone.lightPosition - glm::vec3(0.5, roofHeight, 0.5));
			RenderSimpleMesh(meshes["cone"], shaders["BasicShader"], modelMatrix, cone.color);
		}
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}

void Tema3::DrawScene() {

	for (dancer& dancer : dancers) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(dancer.translateX, 0, dancer.translateZ));
		RenderSimpleMesh(meshes["dancer"], shaders["DiscoShader"], modelMatrix, dancer.color, mapTextures["globe"]);
	}

	// Dance ring
	for (tile& tile : tiles) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(tile.cx - tileSize / 2, 0, tile.cz - tileSize / 2));
		RenderSimpleMesh(meshes["tile"], shaders["DiscoShader"], modelMatrix, tile.color, mapTextures["globe"]);
	}

	DrawRoom();

	// Globe
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, globePosition);
		RenderSimpleMesh(meshes["sphere"], shaders["DiscoShader"], modelMatrix, glm::vec3(0), mapTextures["globe"]);
	}

	// Ground
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -0.1f, 0));
		RenderSimpleMesh(meshes["plane"], shaders["DiscoShader"], modelMatrix, glm::vec3(0.5), mapTextures["disco"]);
	}

	DrawCones();
}

void Tema3::Update(float deltaTimeSeconds)
{
	UpdateDancers(deltaTimeSeconds);
	DrawScene();
}


void Tema3::FrameEnd()
{
	// DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}



void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3& color, Texture2D* texture)
{
	if (!mesh || !shader || !shader->GetProgramID())
		return;

	// Render an object using the specified shader and the specified position
	glUseProgram(shader->program);

	// Bind model matrix
	GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
	glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Bind view matrix
	glm::mat4 viewMatrix = camera->GetViewMatrix();
	int loc_view_matrix = glGetUniformLocation(shader->program, "View");
	glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// Bind projection matrix
	int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
	glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

	glUniform1f(glGetUniformLocation(shader->program, "phase"), phase);

	// Set eye position (camera position) uniform
	glm::vec3 eyePosition = camera->position;
	glUniform3f(glGetUniformLocation(shader->program, "eye_position"), eyePosition.x, eyePosition.y, eyePosition.z);

	// Set material property uniforms (shininess, kd, ks, object color, time) 
	glUniform1i(glGetUniformLocation(shader->program, "material_shininess"), materialShininess);
	glUniform1f(glGetUniformLocation(shader->program, "material_kd"), materialKd);
	glUniform1f(glGetUniformLocation(shader->program, "material_ks"), materialKs);
	glUniform3f(glGetUniformLocation(shader->program, "object_color"), color.x, color.y, color.z);
	glUniform1f(glGetUniformLocation(shader->program, "time"), (GLfloat)Engine::GetElapsedTime());

	glUniform3f(glGetUniformLocation(shader->program, "globe_position"), globePosition.x, globePosition.y, globePosition.z);

	glUniform3fv(glGetUniformLocation(shader->program, "conePos"), 4, glm::value_ptr(conePos[0]));
	glUniform3fv(glGetUniformLocation(shader->program, "coneColor"), 4, glm::value_ptr(coneColor[0]));
	glUniform3fv(glGetUniformLocation(shader->program, "coneDir"), 4, glm::value_ptr(coneDir[0]));

	glUniform3fv(glGetUniformLocation(shader->program, "tilePos"), 64, glm::value_ptr(tilePos[0]));
	glUniform3fv(glGetUniformLocation(shader->program, "tileColor"), 64, glm::value_ptr(tileColor[0]));
	glUniform3fv(glGetUniformLocation(shader->program, "tileDir"), 64, glm::value_ptr(tileDir[0]));

	if (texture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);

		if (mesh == meshes["sphere"]) {
			glUniform1i(glGetUniformLocation(shader->program, "use_texture"), 1);
		}
		else {
			glUniform1i(glGetUniformLocation(shader->program, "use_texture"), 0);
		}
	}

	if (mesh == meshes["quad"]) {
		glUniform1f(glGetUniformLocation(shader->program, "invertNormals"), 1);
	}
	else
	{
		glUniform1f(glGetUniformLocation(shader->program, "invertNormals"), 0);
	}

	if (mesh == meshes["sphere"]) {
		glUniform1f(glGetUniformLocation(shader->program, "isGlobe"), 1);
	}
	else {
		glUniform1f(glGetUniformLocation(shader->program, "isGlobe"), 0);
	}

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->m_VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema3::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{

		if (window->KeyHold(GLFW_KEY_W)) {
			// Translate the camera forward
			camera->MoveForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// Translate the camera to the left
			camera->TranslateRight(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// Translate the camera backward
			camera->MoveForward(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// Translate the camera to the right
			camera->TranslateRight(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			// Translate the camera downward
			camera->TranslateUpward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			// Translate the camera upward
			camera->TranslateUpward(-cameraSpeed * deltaTime);
		}
	}

	if (window->KeyHold(GLFW_KEY_KP_ADD))
	{
		fov += deltaTime * cameraSpeed;

		if (perspective)
		{
			projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, znear, zfar);
		}
	}
	else if (window->KeyHold(GLFW_KEY_KP_SUBTRACT))
	{
		fov -= deltaTime * cameraSpeed;

		if (perspective)
		{
			projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, znear, zfar);
		}
	}
}


void Tema3::OnKeyPress(int key, int mods)
{
	// Add key press event
	if (key == GLFW_KEY_T)
	{
		// Switch shaders
		phase = phase < shaderCount - 1 ? phase + 1 : 0;
	}
	// Switch projections
	if (key == GLFW_KEY_O)
	{
		perspective = false;
		projectionMatrix = glm::ortho(left, right, bottom, top, znear, zfar);
	}

	if (key == GLFW_KEY_P)
	{
		perspective = true;
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, znear, zfar);
	}
}


void Tema3::OnKeyRelease(int key, int mods)
{
	// Add key release event
}


void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// Add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
			// Rotate the camera in first-person mode around
			// OX and OY using `deltaX` and `deltaY`
			camera->RotateFirstPerson_OX(-deltaY * sensivityOY);
			camera->RotateFirstPerson_OY(-deltaX * sensivityOX);
		}

		if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
			// Rotate the camera in third-person mode around
			// OX and OY using `deltaX` and `deltaY`
			camera->RotateThirdPerson_OX(deltaY * sensivityOY);
			camera->RotateThirdPerson_OY(deltaX * sensivityOX);
		}
	}
}


void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button press event
}


void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// Add mouse button release event
}


void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema3::OnWindowResize(int width, int height)
{
}
