#include "Common.hlsli"
#include "Lighting.hlsli"

struct VOutR
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float3 uvw : TEXCOORD1;
    float4 colour : COLOUR;
};

cbuffer rvLightingData //: register(b13) //this buffer is binded to register 13
{
    float4 rvAmbientLightColour;
    DirectionalLight rvDirectionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight rvPointLights[MAX_POINT_LIGHTS];
};

VOutR main(VIn input)
{
    VOutR output;
    output.position = mul(wvp, float4(input.position, 1));
    output.uv = input.uv;
    output.uvw = CalculateReflectionUVW(world, float4(input.position, 1), input.normal, cameraPosition);
    output.colour = float4(CalculateAllLighting(rvAmbientLightColour.xyz, rvDirectionalLights, rvPointLights, float4(input.position, 1), input.normal), 1);
    return output;
}