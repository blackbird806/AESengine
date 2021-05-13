struct VS_INPUT
{
    float2 position : POSITION;
    float2 UV : TEXCOORD;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer CameraBuffer
{
	float4x4 projectionMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    // output.position = mul(float4(input.position.x, input.position.y, 0.0f, 1.0f), projectionMatrix);
    output.position = float4(input.position.x, input.position.y, 0.0f, 1.0f);
    output.color = input.color;
    return output;
}