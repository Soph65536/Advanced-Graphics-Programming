#include "Common.hlsli"

struct VOutS
{
    float4 position : SV_Position;
    float3 uvw : TEXCOORD;
};

VOutS main( VIn input )
{
    VOutS output;
    output.position = mul(wvp, float4(input.position, 1));
    output.uvw = input.position.xyz;
    return output;
}