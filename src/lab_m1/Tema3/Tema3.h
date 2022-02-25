#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema3/GameCamera.h"


namespace m1
{
	class Tema3 : public gfxc::SimpleScene
	{

	public:
		struct tile {
			float cx, cz;
			glm::vec3 color;
		};

		struct lightCone {
			glm::vec3 lightPosition;
			glm::vec3 lightDirection;
			glm::vec3 color;
		};

		struct dancer {
			float translateX, translateZ;
			glm::vec3 color;
			int dirX, dirZ;
			float lastChange;
		};

	public:
		Tema3();
		~Tema3();

		void Init() override;

	private:
		void FrameStart() override;
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void CreateDiscoScene();
		void UpdateDancers(float deltaTimeSeconds);
		void DrawScene();
		void DrawRoom();
		void DrawCones();
		void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3& color, Texture2D* texture = NULL);
		float RandomFloat(float a, float b);
		Texture2D* CreateRandomTexture(unsigned int width, unsigned int height);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	protected:
		implemented::GameCamera* camera;
		glm::mat4 projectionMatrix;
		bool renderCameraTarget;

		float top, bottom, left, right;
		float fov;
		float znear, zfar;
		bool perspective = true;

		std::unordered_map<std::string, Texture2D*> mapTextures;

		glm::vec3 lightPosition;
		glm::vec3 lightDirection;
		unsigned int materialShininess;
		float materialKd;
		float materialKs;

		GLint typeOfLight;
		GLfloat angleOX, angleOY;
		GLfloat cutoffAngle;

		std::vector<dancer>dancers;
		std::vector<tile>tiles;
		std::vector<lightCone>lightCones;

		float dancerSize = 0.25f;
		float tileSize = 1.f;
		float dancerSpeed = 0.5f;
		float cameraSpeed = 3.0f;
		float roofHeight = 4.f;
		float timeChangeDirection = 1.25f;
		int tilesCount = 8;
		int dancersCount = 10;

		int shaderCount = 4;
		float phase;

		glm::vec3 globePosition = glm::vec3(4, 3.5f, 4);

		glm::vec3 conePos[4];
		glm::vec3 coneColor[4];
		glm::vec3 coneDir[4];

		glm::vec3 tilePos[64];
		glm::vec3 tileColor[64];
		glm::vec3 tileDir[64];
	};
}   // namespace m1
