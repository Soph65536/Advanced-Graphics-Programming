struct VIn
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
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
};

VOut main( VIn input )
{
    VOut output;
    output.position = mul(world, float4(input.position, 1));
    output.uv = input.uv;
    output.colour = float4(1, 1, 1, 1);
	return output;
}