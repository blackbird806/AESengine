#ifndef WOB_D3D11DRAW3DSHADER_HPP
#define WOB_D3D11DRAW3DSHADER_HPP

namespace wob
{
	constexpr const char* draw3dVSSource = 
	R"(
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

cbuffer CameraBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};
/*
cbuffer ModelBuffer : register(b1)
{
	float4x4 worldMatrix;
};
*/
VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;

	// Change the position vector to be 4 units for proper matrix calculations.

	output.position = float4(input.position, 1.0f);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	//output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the input color for the pixel shader to use.
	output.color = input.color;
	
	return output;
}
	)";

	constexpr const char* draw3dFSSource = 
	R"(
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return input.color;
}
	)";
}

#endif