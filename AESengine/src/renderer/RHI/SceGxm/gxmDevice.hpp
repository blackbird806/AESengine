#ifndef AES_GXMDEVICE_HPP
#define AES_GXMDEVICE_HPP

#include "core/aes.hpp"
#include "core/error.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "gxmShader.hpp"
#include "gxmTexture.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHIRenderTarget.hpp"

#include "core/platformVita/vitagxm.h"
#include "gxmCompatibilty.h"

namespace aes
{
	extern SceGxmShaderPatcher* gxmShaderPatcher;

	void initializeGraphicsAPI();
	void terminateGraphicsAPI();

	class GxmDevice
	{
		struct IndexBufferInfo
		{
			IndexTypeFormat typeFormat;
			void* buffer = nullptr;
		};

		struct State
		{
			DrawPrimitiveType primitiveType;
			IndexBufferInfo indexBufferInfo;
		};

		public:
		
		GxmDevice() = default;
		GxmDevice(GxmDevice&&) noexcept;
		~GxmDevice();
		GxmDevice& operator=(GxmDevice&&) noexcept;
		
		Result<void> init();
		void destroy();

		// not sure about this name
		void swapBuffers(RHIRenderTarget const& oldBuffer, RHIRenderTarget const& newBuffer);

		// draw calls

		void drawIndexed(uint indexCount, uint indexOffset = 0);

		void beginRenderPass(RHIRenderTarget& rt);
		void endRenderPass();

		// state modification

		void setCullMode(CullMode mode);
		void setDrawPrimitiveMode(DrawPrimitiveType mode);
		
		void setFragmentShader(RHIFragmentShader& fs);
		void setVertexShader(RHIVertexShader& vs);

		Result<void> setVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0);
		Result<void> setIndexBuffer(RHIBuffer& buffer, IndexTypeFormat typeFormat, uint offset = 0);

		// resources binding

		Result<void> bindFragmentUniformBuffer(RHIBuffer& buffer, uint slot);
		Result<void> bindVertexUniformBuffer(RHIBuffer& buffer, uint slot);
		Result<void> bindFragmentTexture(RHITexture& buffer, uint slot);
		Result<void> bindVertexTexture(RHITexture& buffer, uint slot);

		private:

		SceGxmContext* context;
		SceUID vdmRingBufferUid, vertexRingBufferUid, fragmentRingBufferUid, fragmentUsseRingBufferUid;
		void* hostMem;
		State currentState;
	};

	using RHIDevice = GxmDevice;
}

#endif