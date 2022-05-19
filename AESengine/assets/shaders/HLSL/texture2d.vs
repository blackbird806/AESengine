struct VS_INPUT
{
    float2 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer uniformBuffer
{
	float4x4 transformMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    // output.position = float4(input.position.x, input.position.y, 0.0f, 1.0f);
    output.position = mul(float4(input.position.x, input.position.y, 0.0f, 1.0f), transformMatrix);
    output.uv = input.uv;
    return output;
}