#include "Header.hlsli"

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 BaseColor = txDiffuse.Sample(samplerLinear, input.Texcoord);
    
    return BaseColor;
}