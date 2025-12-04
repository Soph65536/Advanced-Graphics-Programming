#include "Common.hlsli"

Texture2D texture0 : register(t0);
TextureCube skybox0 : register(t1);
sampler sampler0;

struct FInR
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
    float3 uvw : TEXCOORD1;
    float4 colour : COLOUR;
};

float4 main(FInR input) : SV_TARGET
{
    float4 sampled = texture0.Sample(sampler0, input.uv);
    float4 reflectedSampled = skybox0.Sample(sampler0, input.uvw);
    float4 combined = (input.colour * sampled * 0.3) + (reflectedSampled * 0.7);
    clip(sampled.a < 0.1f ? -1 : 1); //dont render transparency
    return saturate(combined);
}