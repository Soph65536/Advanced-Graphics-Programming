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

cbuffer CBufferFS
{
    float reflectiveness;
    float3 packing;
};

float4 main(FInR input) : SV_TARGET
{
    float4 sampled = texture0.Sample(sampler0, input.uv);
    float4 reflectedSampled = skybox0.Sample(sampler0, input.uvw);
    float4 combined = (input.colour * sampled * (1.0 - reflectiveness)) + (reflectedSampled * reflectiveness);
    clip(sampled.a < 0.1f ? -1 : 1); //dont render transparency
    return saturate(combined);
}