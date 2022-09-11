#ifndef AES_GXMDEVICE_HPP
#define AES_GXMDEVICE_HPP

#include "core/aes.hpp"
#include "core/error.hpp"

// @review only include needed gxm headers
#include <vitasdk.h>
#include "gxmCompatibilty.h"

namespace aes
{
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
		GxmDevice(GxmDevice&&) noexcept;
		~GxmDevice();
		GxmDevice& operator=(GxmDevice&&) noexcept;
		
		Result<void> init();
		void destroy();

		// draw calls

		void drawIndexed(uint indexCount, uint indexOffset = 0);

		// state modification

		void setCullMode(CullMode mode);
		void setDrawPrimitiveMode(DrawPrimitiveType mode);
		
		void setFragmentShader(RHIFragmentShader& fs);
		void setVertexShader(RHIVertexShader& vs);

		Result<void> setVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0);
		Result<void> setIndexBuffer(RHIBuffer& buffer, IndexTypeFormat typeFormat, uint offset = 0);

		// resources binding

		Result<void> bindFragmentUniformBuffer(RHIBUffer& buffer, uint slot);
		Result<void> bindVertexUniformBuffer(RHIBUffer& buffer, uint slot);
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