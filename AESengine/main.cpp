#include <fmt/format.h>
#include <iostream>
#include <glm/gtx/transform.hpp>
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "renderer/D3D11Model.hpp"
#include "renderer/immediateRenderer.hpp"
#include "renderer/FontManager.hpp"

class Game : public aes::Engine
{

public:
	
	Game(InitInfo const& info) : Engine(info)
	{
	
	}
	aes::D3D11Model model, model2;

	void start() override
	{
		aes::FontManager fm;
		auto r = fm.init();
		if (!r)
			AES_WARN("fm init failed : {}", r.error());

		AES_LOG("start");
		model = aes::createCube();
		//aes::ImmediateRenderer2D::Instance().init();
		model2 = aes::createCube();
		model.toWorld = glm::scale(model.toWorld, { 1.0f, 2.0f, 1.0f });
		model2.toWorld = glm::scale(model2.toWorld, { 2.0f, 1.0f, 1.0f });
		model2.toWorld *= glm::translate(model2.toWorld, { 0.0f, 1.0f, 3.0f });
		mainCamera.pos = { 0.0, 0.0, -5.0 };
		getMousePos(lastMousePosX, lastMousePosY);
	}

	float speed = 5.0f, sensitivity = 50.f;
	float lastMousePosX, lastMousePosY;
	glm::vec3 direction = {0.0, 0.0, 1.0};
	float yaw = 91, pitch = 2;

	void update(double dt) override
	{
		glm::vec4 movePos = { 0.0f, 0.f, 0.f, 0.0f };
		if (getKeyState(aes::Key::W) == aes::InputState::Down)
		{
			movePos.z += speed * dt;
		}

		if (getKeyState(aes::Key::S) == aes::InputState::Down)
		{
			movePos.z -= speed * dt;
		}

		if (getKeyState(aes::Key::D) == aes::InputState::Down)
		{
			movePos.x += speed * dt;
		}

		if (getKeyState(aes::Key::A) == aes::InputState::Down)
		{
			movePos.x -= speed * dt;
		}

		if (getKeyState(aes::Key::E) == aes::InputState::Down)
		{
			movePos.y += speed * dt;
		}

		if (getKeyState(aes::Key::Q) == aes::InputState::Down)
		{
			movePos.y -= speed * dt;
		}

		mainCamera.pos += glm::vec3(movePos * mainCamera.viewMatrix);

		if (getKeyState(aes::Key::RClick) == aes::InputState::Down)
		{
			float mx, my;
			getMousePos(mx, my);

			float xoffset = mx - lastMousePosX;
			float yoffset = my - lastMousePosY; // reversed since y-coordinates range from bottom to top

			lastMousePosX = mx;
			lastMousePosY = my;

			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		}
		else
		{
			getMousePos(lastMousePosX, lastMousePosY);
		}
		mainCamera.lookAt(mainCamera.pos + glm::normalize(direction));

		// draw crosshair
		{
			float ex = 0.0055f;
			float csx = 0.025f;

			uint windowWidth, windowHeight;
			mainWindow.getScreenSize(windowWidth, windowHeight);
			float aspect = (float) windowWidth / (float) windowHeight;
			mainCamera.projMatrix = glm::perspectiveLH_ZO(glm::radians(45.0f), aspect, 0.0001f, 1000.0f);

			float ey = ex * aspect;
			float csy = csx * aspect;
			//aes::ImmediateRenderer2D::Instance().drawLine({ 0, ey }, { 0, ey + csy }, { 0.0f, 1.0f, 0.0f, 1.0f });
			//aes::ImmediateRenderer2D::Instance().drawLine({ 0, -ey }, { 0, -ey - csy }, { 0.0f, 1.0f, 0.0f, 1.0f });
			//aes::ImmediateRenderer2D::Instance().drawLine({ ex, 0 }, { ex + csx, 0 }, { 0.0f, 1.0f, 0.0f, 1.0f });
			//aes::ImmediateRenderer2D::Instance().drawLine({ -ex, 0 }, { -ex - csx, 0 }, { 0.0f, 1.0f, 0.0f, 1.0f });
		}
	}

	void draw() override
	{
		model.render();
		model2.render();
		//aes::ImmediateRenderer2D::Instance().draw();
	}

};

int main()
{
	fmt::print("hello {}\n", "world");
	Game game({ .appName = "aes cube" });
	game.init();
	game.run();
}

