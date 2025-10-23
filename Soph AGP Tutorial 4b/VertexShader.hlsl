struct VIn
{
    float3 position : POSITION;
    float4 colour : COLOUR;
};

struct VOut
{
    float4 position : SV_Position;
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
    output.colour = input.colour;
	return output;
}