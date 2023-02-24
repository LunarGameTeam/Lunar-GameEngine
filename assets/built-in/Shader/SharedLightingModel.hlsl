#ifndef __SHARED_LIGHTING_MODEL__
#define __SHARED_LIGHTING_MODEL__

#include "SharedCBuffer.hlsl"

void LambertLighting(float3 Lo, float3 N, float3 Li, float3 lightColor, float3 pixelColor, float attenuation, out float3 outColor)
{
	outColor += lightColor * pixelColor * dot(N, Li) * attenuation;
	outColor = saturate(outColor);
}

#endif