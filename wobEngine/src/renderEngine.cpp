#include "renderEngine.hpp"

using namespace wob;

RenderEngine::RenderEngine(InitInfo const& info) : Engine(info)
{

}

void RenderEngine::start()
{
	initializeGraphicsAPI();
	// create device
	device.init();
	device.setCullMode(CullMode::Clockwise);

	mainWindow->addResizeCallback({ [](uint w, uint h, void* app) {
		RenderEngine* engine = (RenderEngine*)app;
		engine->buildSwapchain();
		}, this });

	buildSwapchain();
}

void RenderEngine::draw()
{
	device.clearSwapchain(swapchain);

	device.beginRenderPass(swapchain);
	mainDrawPass();
	device.endRenderPass();

	device.swapBuffers(swapchain);
}

void RenderEngine::buildSwapchain()
{
	SwapchainDescription swDesc = {};
	swDesc.count = 2;
	mainWindow->getScreenSize(swDesc.width, swDesc.height);
	swDesc.multisampleMode = MultisampleMode::None;
	swDesc.window = mainWindow->getHandle();
	swDesc.format = RHIFormat::R8G8B8A8_Uint;
	swDesc.depthFormat = RHIFormat::D24_S8_Uint;
	swapchain = device.createSwapchain(swDesc).value();
}
