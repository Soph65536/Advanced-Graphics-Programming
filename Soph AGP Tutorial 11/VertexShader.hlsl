#include "Common.hlsli"
#include "Lighting.hlsli"

VOut main( VIn input )
{
    VOut output;
    output.position = mul(wvp, float4(input.position, 1));
    output.uv = input.uv;
    output.colour = float4(CalculateAllLighting(ambientLightColour.xyz, directionalLights, pointLights, float4(input.position, 1), input.normal), 1);
	return output;
}