cbuffer PerObjectCB : register(b12) //this buffer is binded to register 12
{
    matrix world;
    matrix wvp;
};

cbuffer PerFrame : register(b11)
{
    float3 cameraPosition;
    float padding;
}

struct VIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VOut
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float4 colour : COLOUR;
};

struct FIn
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float4 colour : COLOUR;
};