struct VIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float norm : NORMAL;
};

struct VOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float4 colour : COLOUR;
};

cbuffer PerObjectCB
{
    matrix world;
    float4 ambientLightColour;
    float4 directionalLightColour;
    float4 directionalLightDirection;
    float4 pointLightPosition;
    float4 pointLightColour;
    float pointLightStrength;
};

VOut main( VIn input )
{
    VOut output;
    output.position = mul(world, float4(input.position, 1));
    output.uv = input.uv;
    
    //lighting
    //directional light
    float diffuseAmount = saturate(dot(directionalLightDirection.xyz, input.norm)); //saturate clamps all values to between 0 and 1
    float3 directionalFinal = directionalLightColour * diffuseAmount;
    //point light
    float3 pointLightDirection = normalize(pointLightPosition - input.position);
    float pointLightDistance = length(pointLightPosition - input.position);
    float pointLightAttenuation = pointLightStrength / (pointLightDistance * pointLightDistance + pointLightStrength); //attenuation = strength / (distance^2 + strength)
    float pointAmount = saturate(dot(pointLightDirection.xyz, input.norm) * pointLightAttenuation);
    float3 pointFinal = pointLightColour * pointAmount;
    
    output.colour = saturate(ambientLightColour + float4(directionalFinal, 1) + float4(pointFinal, 1));
	return output;
}