#include "Common.hlsli"
#include "Lighting.hlsli"

struct VOutR
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float3 uvw : TEXCOORD1;
    float4 colour : COLOUR;
};

VOutR main(VIn input)
{
    VOutR output;
    output.position = mul(wvp, float4(input.position, 1));
    output.uv = input.uv;
    output.uvw = CalculateReflectionUVW(world, float4(input.position, 1), input.normal, cameraPosition);
    output.colour = float4(CalculateAllLighting(ambientLightColour.xyz, directionalLights, pointLights, float4(input.position, 1), input.normal), 1);
    return output;
}