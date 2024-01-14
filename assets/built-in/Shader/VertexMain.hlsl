#ifndef __VERTEX_MAIN__
#define __VERTEX_MAIN__

#include "SharedCBuffer.hlsl"


BaseFragment VSMain(BaseVertex input, uint inst : SV_InstanceID)
{
    BaseFragment output;
    uint instanceID = input.instancemessage.x;
	// Change the position vector to be 4 units for proper matrix calculations.
    float4 position = float4(input.position, 1.0);
	matrix worldMatrix = RoWorldMatrixDataBuffer[instanceID];

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(position, worldMatrix);
	output.viewDir = normalize(cCamPos - output.position.xyz);
    output.position = mul(output.position, cViewMatrix);
	//output.depthLinear = (output.position.z - cNearFar.x)/ (cNearFar.y - cNearFar.x);
	output.depthLinear = 0;
   	output.position = mul(output.position, cProjectionMatrix);
	
	// Calculate the position of the vertice as viewed by the light source.
    output.worldPosition = mul(position, worldMatrix);

	// Store the texture coordinates for the pixel shader.
	output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}

#endif