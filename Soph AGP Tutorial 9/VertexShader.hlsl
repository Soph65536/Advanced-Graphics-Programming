#define MAX_POINT_LIGHTS 8

struct PointLight
{
    float4 position;
    float4 colour;

    float strength;
    bool enabled;
    float padding;
};
    
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
    PointLight pointLights[MAX_POINT_LIGHTS];
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
    float3 pointFinal = float3(0, 0, 0);
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        if (!pointLights[i].enabled)
            continue;
        
        float3 pointLightDirection = normalize(pointLights[i].position - input.position);
        float pointLightDistance = length(pointLights[i].position - input.position);
        float pointLightAttenuation = pointLights[i].strength / (pointLightDistance * pointLightDistance + pointLights[i].strength); //attenuation = strength / (distance^2 + strength)
        float pointAmount = saturate(dot(pointLightDirection.xyz, input.norm) * pointLightAttenuation);
        pointFinal += pointLights[i].colour * pointAmount;
    }
    
    output.colour = saturate(ambientLightColour + float4(directionalFinal, 1) + float4(pointFinal, 1));
	return output;
}