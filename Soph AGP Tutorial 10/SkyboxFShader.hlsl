TextureCube cube0;
sampler sampler0;

struct FIn
{
    float4 position : SV_Position;
    float3 uvw : TEXCOORD;
};

float4 main(FIn input) : SV_TARGET
{
    float4 sampled = cube0.Sample(sampler0, input.uvw);
    return sampled;
}