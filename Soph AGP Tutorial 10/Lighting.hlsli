#define MAX_POINT_LIGHTS 8
#define MAX_DIRECTIONAL_LIGHTS 4

struct DirectionalLight
{
    float4 transposedDirection;
    float4 colour;
    
    bool enabled;
    float3 padding;
};

struct PointLight
{
    float4 position;
    float4 colour;

    float strength;
    bool enabled;
    float2 padding;
};

cbuffer lightingData : register(b13) //this buffer is binded to register 13
{
    float4 ambientLightColour;
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
};


float3 CalculateReflectionUVW(matrix world, float4 vertexPosition, float3 vertexNormal, float3 cameraPosition)
{
    //vertex position in worldspace
    float3 wPosition = mul(world, vertexPosition);
    //surface normal in worldspace
    float3 wNormal = normalize(mul(world, float4(vertexNormal, 0)));
    //obtain eye vector
    float3 eyeDirection = normalize(cameraPosition - wPosition);
    //compute reflection vector, below is equivalent to (2 * dot(eyeDirection, wNormal) * wNormal - eyeDir);
    return reflect(-eyeDirection, wNormal);
}


float3 CalculateDirectionalLights(DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS], float3 vNormal) //vnormal is normal from the vertex shader input
{
    float3 directionalFinal = float3(0, 0, 0);
    for (int i = 0; i < MAX_DIRECTIONAL_LIGHTS; ++i)
    {
        if (!directionalLights[i].enabled)
            continue;
        
        float diffuseAmount = saturate(dot(directionalLights[i].transposedDirection.xyz, vNormal)); //saturate clamps all values to between 0 and 1
        directionalFinal += directionalLights[i].colour * diffuseAmount;
    }
    return directionalFinal;
}

float3 CalculatePointLights(PointLight pointLights[MAX_POINT_LIGHTS], float4 vPosition, float3 vNormal) //again vPosition and vNormal are inputs from the vertex shader
{
    float3 pointFinal = float3(0, 0, 0);
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        if (!pointLights[i].enabled)
            continue;
        
        float3 pointLightDirection = normalize(pointLights[i].position - vPosition);
        float pointLightDistance = length(pointLights[i].position - vPosition);
        float pointLightAttenuation = pointLights[i].strength / (pointLightDistance * pointLightDistance + pointLights[i].strength); //attenuation = strength / (distance^2 + strength)
        float pointAmount = saturate(dot(pointLightDirection.xyz, vNormal) * pointLightAttenuation);
        pointFinal += pointLights[i].colour * pointAmount;
    }
    return pointFinal;
}

float3 CalculateAllLighting(float3 ambientColour, DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS], PointLight pointLights[MAX_POINT_LIGHTS], float4 vPosition, float3 vNormal)
{
    float3 directionalTotal = CalculateDirectionalLights(directionalLights, vNormal);
    float3 pointTotal = CalculatePointLights(pointLights, vPosition, vNormal);
    return saturate(ambientColour + directionalTotal + pointTotal);
}