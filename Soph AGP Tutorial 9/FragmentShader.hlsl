#include "Common.hlsli"

Texture2D texture0;
sampler sampler0;

float4 main(FIn input) : SV_TARGET
{
    float4 sampled = texture0.Sample(sampler0, input.uv);
    clip(sampled.a < 0.1f ? -1 : 1); //dont render transparency
    return input.colour * sampled;
}