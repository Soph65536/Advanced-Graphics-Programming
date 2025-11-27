#include "Common.hlsli"
#include "Lighting.hlsli"

struct VOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float4 colour : COLOUR;
};

cbuffer PerObjectCB : register(b12) //this buffer is binded to register 12
{
    matrix world;
};

VOut main( VIn input )
{
    VOut output;
    output.position = mul(world, float4(input.position, 1));
    output.uv = input.uv;
    output.colour = float4(CalculateAllLighting(ambientLightColour.xyz, directionalLights, pointLights, float4(input.position, 1), input.normal), 1);
	return output;
}