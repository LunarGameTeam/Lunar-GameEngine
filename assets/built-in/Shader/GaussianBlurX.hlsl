////////////////////////////////////////////////////////////////////////////////
// Filename: depth.vs
////////////////////////////////////////////////////////////////////////////////
#include "SharedCBuffer.hlsl"
#include "SharedSampler.hlsl"
//////////////
// TYPEDEFS //
//////////////

cbuffer MaterialBuffer : register(b3)
{
	float2 _viewTargetDimensions;
};
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
BaseFragment VSMain(BaseVertex input) 
{
    BaseFragment output;     
    output.position = float4(input.position, 1);
    output.normal = float4(input.normal, 0);
    output.uv = input.uv;
    output.color = input.color;
	return output;
}
static const float KernelOffsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
static const float BlurWeights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };
////////////////////////////////////////////////////////////////////////////////
// Filename: depth.ps
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMain(BaseFragment input) : SV_TARGET
{
   float3 textureColor = float3(1.0f, 0.0f, 0.0f);
    float2 uv = input.uv;
    if (uv.x > (1 + 0.005f))
    {
        textureColor = _MainTex.Sample(_ClampSampler, uv).xyz * BlurWeights[0];
        for (int i = 1; i < 3; i++)
        {
            float2 normalizedOffset = float2(KernelOffsets[i], 0.0f) / _viewTargetDimensions.x;
            textureColor += _MainTex.Sample(_ClampSampler, uv + normalizedOffset).xyz * BlurWeights[i];
            textureColor += _MainTex.Sample(_ClampSampler, uv - normalizedOffset).xyz * BlurWeights[i];
        }
    }
    else if (uv.x <= (1 - 0.005f))
    {
        textureColor = _MainTex.Sample(_ClampSampler, uv).xyz;
    }

    // Artificially increase the workload to simulate a more complex shader.
    const float3 textureColorOrig = textureColor;
    for (uint i = 0; i < 3; i++)
    {
        textureColor += textureColorOrig;
    }

    if (3 > 0)
    {
        textureColor /= 3 + 1;
    }

    return float4(textureColor, 1.0);
}
