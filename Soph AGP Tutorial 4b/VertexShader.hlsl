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
    float3 pos;
    float padding;
};

VOut main( VIn input )
{
    VOut output;
    output.position = float4(input.position + pos, 1);
    output.colour = input.colour;
	return output;
}