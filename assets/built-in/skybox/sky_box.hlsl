////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////
#include "assets/built-in/SharedCBuffer.hlsl"
#include "assets/built-in/SharedSampler.hlsl"


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
BaseFragment VSMain(BaseVertex input, uint inst : SV_InstanceID)
{
    BaseFragment output;
	// Change the position vector to be 4 units for proper matrix calculations.
    float4 position = float4(input.position, 1.0);
	matrix worldMatrix = objectBuffers[inst].worldMatrix;
	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.normal = input.normal;
    return output;
}
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMain(BaseFragment input) : SV_TARGET
{
    float3 texColor = _SkyTex.Sample(SampleTypeClamp, normalize(input.normal)).rgb;
    return float4(texColor, 1.0f);
}