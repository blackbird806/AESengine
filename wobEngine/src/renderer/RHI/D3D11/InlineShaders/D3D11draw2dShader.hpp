#ifndef WOB_D3D11DRAW2DSHADER_HPP
#define WOB_D3D11DRAW2DSHADER_HPP

namespace wob
{
	constexpr const char* draw2dVSSource = 
	R"(
		// https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_dx11.cpp
		struct VS_INPUT
		{
			float2 position : POSITION;
			float4 color : COLOR0;
			float2 uv : TEXCOORD0;
		};

		struct VS_OUTPUT
		{
			float4 position : SV_POSITION;
			float4 color : COLOR0;
			float2 uv : TEXCOORD0;
		};

		cbuffer uniformBuffer
		{
			float4x4 transformMatrix;
		};

		VS_OUTPUT main(VS_INPUT input)
		{
			VS_OUTPUT output;
			output.position = mul(transformMatrix, float4(input.position.xy, 0.f, 1.f));
			output.color = input.color;
			output.uv = input.uv;
			return output;
		}
	)";

	constexpr const char* draw2dFSSource = 
	R"(
		struct VS_OUTPUT
		{
			float4 position : SV_POSITION;
			float4 color : COLOR0;
			float2 uv : TEXCOORD0;
		};

		SamplerState samplerState;
		Texture2D texture0;

		float4 main(VS_OUTPUT input) : SV_TARGET
		{
			return input.color * texture0.Sample(samplerState, input.uv);
		}
	)";
}

#endif