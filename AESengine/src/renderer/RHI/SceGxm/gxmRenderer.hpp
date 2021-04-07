#ifndef GXMRENDERER_HPP
#define GXMRENDERER_HPP

#include "core/window.hpp"
#include "renderer/camera.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "renderer/RHI/RHIBuffer.hpp"

#include <vitasdk.h>
#include "gxmCompatibilty.h"

namespace aes
{
	auto constexpr vita_display_width = 960;
	auto constexpr vita_display_height = 544;
	auto constexpr vita_display_stride_in_pixels = 1024;
	auto constexpr vita_display_pixel_format = SCE_DISPLAY_PIXELFORMAT_A8B8G8R8;
	auto constexpr vita_display_color_format = SCE_GXM_COLOR_FORMAT_A8B8G8R8;
	auto constexpr vita_display_buffer_count = 3;
	auto constexpr vita_msaa_mode = SCE_GXM_MULTISAMPLE_NONE;
	auto constexpr vita_display_max_pending_swaps = 2;

	// allocate memory for GPU use
	void* graphicsAlloc(SceKernelMemBlockType type, uint32_t size, uint32_t alignement, uint32_t attribs, SceUID* uid, const char* name = "default");
	void graphicsFree(SceUID uid);

	struct BasicVertex {
		float x;
		float y;
		float z;
		uint32_t color;
	};

	class GxmRenderer
	{
		public:

			struct State
			{
				DrawPrimitiveType primitiveType;
				IndexBufferInfo indexBufferInfo;
			};

			static GxmRenderer& instance();
		
			void init(Window& windowHandle);
			void destroy();

			void startFrame(Camera const& cam);
			void endFrame();
			
			void bindBuffer(RHIBuffer& buffer, uint slot);
			void bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset = 0);
			void bindIndexBuffer(RHIBuffer& buffer, TypeFormat typeFormat, uint offset = 0);

			void setDrawPrimitiveMode(DrawPrimitiveType mode);
			void drawIndexed(uint indexCount);

		private:

			struct IndexBufferInfo
			{
				TypeFormat typeFormat;
				void* buffer;
			};
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
			SceGxmShaderPatcherId basicVertexProgramId;
			SceGxmShaderPatcherId basicFragmentProgramId;

			SceGxmVertexProgram* clearVertexProgram = nullptr;
			SceGxmFragmentProgram* clearFragmentProgram = nullptr;
			SceUID clearVerticesUid;
			SceUID clearIndicesUid;

			glm::vec2* clearVertices;
			uint16_t* clearIndices;
			BasicVertex* basicVertices;
			uint16_t* basicIndices;
			SceGxmProgramParameter const* wvpParam;

			SceGxmVertexProgram* basicVertexProgram = nullptr;
			SceGxmFragmentProgram* basicFragmentProgram = nullptr;

			SceUID basicVerticesUid, basicIndiceUid;

			uint32_t backBufferIndex = 0;
			uint32_t frontBufferIndex = 0;
	};
	
	using RHIRenderContext = GxmRenderer;
}

#endif
