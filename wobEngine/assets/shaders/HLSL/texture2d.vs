// https://github.com/ocornut/imgui/blob/master/backends/imgui_impl_dx11.cpp
struct VS_INPUT
{
	float2 position : POSITION;
	float2 uv : TEXCOORD;
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
	output.position = mul(transformMatrix, input.position.xy, 0.f, 1.f);
	output.color = input.color;
	output.uv = input.uv;
	return output;
}