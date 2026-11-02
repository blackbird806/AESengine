#ifndef WOB_RENDER_ENGINE
#define WOB_RENDER_ENGINE

#include "engine.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "renderer/RHI/RHISwapchain.hpp"

namespace wob
{
	class RenderEngine : public Engine
	{
	public:

		RenderEngine(InitInfo const& info);

		virtual void start() override;
		virtual void draw() override;

		virtual void mainDrawPass() {}

	protected:

		void buildSwapchain();

		RHIDevice device;
		RHISwapchain swapchain;
	};
}

#endif