struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D tex;
SamplerState samplerState;

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return tex.Sample(samplerState, input.uv);
}
