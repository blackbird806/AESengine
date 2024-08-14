struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
	float2 uv : TEXCOORD0;
};

Texture2D texture;
SamplerState samplerState;

float4 main(VS_OUTPUT input) : SV_TARGET
{
	return input.color * texture.Sample(samplerState, input.uv);
}
