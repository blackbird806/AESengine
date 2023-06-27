#ifndef AES_GXMRENDERER_HPP
#define AES_GXMRENDERER_HPP

#include "core/window.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "renderer/RHI/RHIBuffer.hpp"

#include "../RHIBuffer.hpp"
#include "../RHITexture.hpp"
#include "../RHIShader.hpp"
#include "../RHISampler.hpp"
#include <glm/glm.hpp>
#include <display.h>
#include "gxmCompatibilty.h"
#include "gxmMemory.hpp"

namespace aes
{
	auto constexpr vita_display_width = 960;
	auto constexpr vita_display_height = 544;
	auto constexpr vita_display_stride_in_pixels = 1024;
	auto constexpr vita_display_pixel_format = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
	// auto constexpr vita_display_color_format = SCE_GXM_COLOR_FORMAT_A8B8G8R8;
	auto constexpr vita_display_color_format = SCE_GXM_COLOR_FORMAT_U8U8U8U8_RGBA;
	auto constexpr vita_display_buffer_count = 3;
	auto constexpr vita_msaa_mode = SCE_GXM_MULTISAMPLE_NONE;
	auto constexpr vita_display_max_pending_swaps = 2;
	auto constexpr vita_texture_max_resolution = 4096;
	auto constexpr vita_texture_max_mip_count = 13;

	class GxmRenderer
	{
			struct IndexBufferInfo
			{
				IndexTypeFormat typeFormat;
				void* buffer;
			};

		public:

			struct State
			{
				DrawPrimitiveType primitiveType;
				IndexBufferInfo indexBufferInfo;
			};

			static GxmRenderer& instance();
		
			void init(Window& windowHandle);
			void destroy();

			void startFrame();
			void endFrame();
			
			void bindVSUniformBuffer(RHIBuffer& buffer, uint slot);
			void bindFSUniformBuffer(RHIBuffer& buffer, uint slot);

			void bindVertexTexture(RHITexture& tex, uint index);
			void bindFragmentTexture(RHITexture& tex, uint index);

			void bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0);
			void bindIndexBuffer(RHIBuffer& buffer, IndexTypeFormat typeFormat, uint offset = 0);

			void setFragmentShader(RHIFragmentShader& fs);
			void setVertexShader(RHIVertexShader& vs);

			void setFragmentSampler(RHISampler& sampler, uint index);

			void setDrawPrimitiveMode(DrawPrimitiveType mode);
			void drawIndexed(uint indexCount, uint indexOffset = 0);

			SceGxmShaderPatcher* getShaderPatcher() const;
			SceGxmContext* getContext() const { return context; };

		// private:

			static GxmRenderer* inst;

			State currentState;

			SceGxmContext* context;
			void* hostMem;
			SceGxmRenderTarget* renderTarget;
			SceUID vdmRingBufferUid, vertexRingBufferUid, fragmentRingBufferUid, fragmentUsseRingBufferUid;

			void* displayBufferData[vita_display_buffer_count];
			SceUID displayBufferUid[vita_display_buffer_count];
			SceGxmColorSurface displaySurface[vita_display_buffer_count];
			SceGxmSyncObject* displayBufferSync[vita_display_buffer_count];

			SceGxmDepthStencilSurface depthSurface;
			SceUID depthBufferUid;

			SceUID patcherBufferUid;
			SceUID patcherVertexUsseUid;
			SceUID patcherFragmentUsseUid;
			SceGxmShaderPatcher* shaderPatcher = nullptr;

			SceGxmShaderPatcherId clearVertexProgramId;
			SceGxmShaderPatcherId clearFragmentProgramId;

			SceGxmVertexProgram* clearVertexProgram = nullptr;
			SceGxmFragmentProgram* clearFragmentProgram = nullptr;
			SceUID clearVerticesUid;
			SceUID clearIndicesUid;

			glm::vec2* clearVertices;
			uint16_t* clearIndices;

			uint32_t backBufferIndex = 0;
			uint32_t frontBufferIndex = 0;
	};
	
	using RHIRenderContext = GxmRenderer;
}

#endif
