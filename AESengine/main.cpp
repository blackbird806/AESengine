#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "renderer/RHI/model.hpp"
#include "core/os.hpp"

#include "core/color.hpp"

class Game : public aes::Engine
{

public:
	
	Game(InitInfo const& info) : Engine(info)
	{
		AES_LOG("Game initialized");
	}

	void start() override
	{
		AES_PROFILE_FUNCTION();
		AES_LOG("start");

		mainCamera.pos = { 0.0, 0.0, -5.0 };
		getViewportMousePos(lastMousePosX, lastMousePosY);
	}

	float speed = 5.0f, sensitivity = 50.f;
	float lastMousePosX, lastMousePosY;
	glm::vec3 direction = {0.0, 0.0, 1.0};
	float yaw = 91, pitch = 2;
	
	void update(double dt) override
	{
		 AES_PROFILE_FUNCTION();
		/*
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
		 float mx, my;
		 getViewportMousePos(mx, my);
		
		 if (getKeyState(aes::Key::RClick) == aes::InputState::Down)
		 {
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
		 	getViewportMousePos(lastMousePosX, lastMousePosY);
		 }
		 mainCamera.lookAt(mainCamera.pos + glm::normalize(direction));

		 {
		 	float const ex = 0.0055f;
		 	float const csx = 0.025f;

		 	uint windowWidth, windowHeight;
		 	mainWindow->getScreenSize(windowWidth, windowHeight);
		 	float const aspect = (float)windowWidth / (float)windowHeight;
		 	mainCamera.projMatrix = glm::perspectiveLH_ZO(glm::radians(45.0f), aspect, 0.0001f, 1000.0f);
		 }
		 */
	}

	void draw() override
	{
		AES_PROFILE_FUNCTION();
	}
};

int main()
{
#ifdef __vita__
	std::ofstream logFile("ux0:log/AES_log.txt");
	//aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
#else
	std::ofstream logFile("AES_log.txt");
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
#endif
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(logFile));

	Game game({
		.appName = "aes cubes"
	});

	game.init();
	game.run();

	return 0;
}
