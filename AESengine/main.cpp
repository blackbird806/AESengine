#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "renderer/RHI/model.hpp"
#include "core/os.hpp"

#include "core/color.hpp"
#include "UI/UI.hpp"

#ifdef _WIN32
	#include "renderer/Renderer2D.hpp"
	#include "renderer/FontManager.hpp"
#endif

class Game : public aes::Engine
{

public:
	aes::FontManager fm;
	aes::ui::ImguiContext uiContext;
#ifdef _WIN32
	aes::Model model;
	aes::Renderer2D renderer2d;
#endif
	
	Game(InitInfo const& info) : Engine(info), uiContext(std::pmr::get_default_resource())
	{
		AES_LOG("Game initialized");
	}

	void start() override
	{
		AES_PROFILE_FUNCTION();
		AES_LOG("start");
		auto rcube = aes::createCube();
		renderer2d.init();
		fm.init();
		// auto rcube2 = aes::createCube();
		
		// if (!rcube || !rcube2)
		// {
		// 	AES_LOG_ERROR("Failed to create cubes");
		// 	return;
		// }
		
		 model = std::move(rcube.value());
		// model2 = std::move(rcube2.value());
		
		 model.toWorld = glm::scale(model.toWorld, { 1.0f, 2.0f, 1.0f });
		// model2.toWorld = glm::scale(model2.toWorld, { 2.0f, 1.0f, 1.0f });
		// model2.toWorld *= glm::translate(model2.toWorld, { 0.0f, 1.0f, 3.0f });
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

		uiContext.startFrame();
		getScreenMousePos(uiContext.mouse_x, uiContext.mouse_y);
		if (uiContext.button(1, 200, 100))
		{
			AES_LOG("yay");
		}
		renderer2d.drawFillRect({ {0.0f, 0.0f}, { 0.1f, 0.1f} }, { 1.0f, 0.0f, 0.0f, 1.0f });
		fm.drawString({ fmt::format("{} {}", uiContext.mouse_x, uiContext.mouse_y), {0.1, 0.1}, {1.0, 0.0, 0.0, 1.0}, 64.0f });
		fm.drawString({ fmt::format("{} {}", mx, my), {0.1, 0.005}, {1.0, 0.0, 0.0, 1.0}, 64.0f });
		fm.drawString({ fmt::format("{}", mainCamera.pos), {0, 0.3}, {1.0, 0.0, 0.0, 1.0}, 64.0f });
	}

	void draw() override
	{
		AES_PROFILE_FUNCTION();
		model.render();

		uint winSizeX, winSizeY;
		mainWindow->getScreenSize(winSizeX, winSizeY);
		
		for (auto const& cmd : uiContext.getCommandList())
		{
			if (std::holds_alternative<aes::ui::Rect>(cmd.type))
			{
				auto const rect = std::get<aes::ui::Rect>(cmd.type);
				renderer2d.drawFillRect(aes::Rect{ { (float)rect.x / winSizeX, (float)rect.y / winSizeY }, {(float)(rect.x + rect.w)/ winSizeX, (float)(rect.y + rect.h) / winSizeY } }, cmd.color.toVec4());
			}
		}
		renderer2d.draw();
		fm.render();
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
