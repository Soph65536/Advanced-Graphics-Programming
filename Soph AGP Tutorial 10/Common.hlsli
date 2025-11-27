struct VIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float normal : NORMAL;
};

struct FIn
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float4 colour : COLOUR;
};