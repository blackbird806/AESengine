#include <iostream>
#include "core/aes.hpp"
#include "core/debug.hpp"
// #include "test/tests.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "renderer/RHI/RHIRenderTarget.hpp"

using namespace aes;

int main()
{
#ifdef __vita__
	aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
#else
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
#endif

	initializeGraphicsAPI();

	RHIDevice device;
	device.init();

	RenderTargetDescription rtDesc{};
	rtDesc.textureDesc.width = 960;
	rtDesc.textureDesc.height = 544;
	rtDesc.textureDesc.mipsLevel = 0;
	rtDesc.textureDesc.format = RHIFormat::R8G8B8A8_Uint;
	rtDesc.textureDesc.usage = MemoryUsage::Default;
	rtDesc.textureDesc.cpuAccess = CPUAccessFlags::None;
	rtDesc.multisampleMode = MultisampleMode::None;

	RHIRenderTarget renderTargets[2];
	renderTargets[0].init(rtDesc);
	renderTargets[1].init(rtDesc);

	

	return 0;
}
