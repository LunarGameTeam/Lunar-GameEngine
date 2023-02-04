#ifndef __SHARED_LIGHTING_MODEL__
#define __SHARED_LIGHTING_MODEL__

#include "SharedCBuffer.hlsl"

void LambertLighting(BaseFragment input, float3 Lo, float3 N, float3 Li, float3 Lradiance, float attenuation, out float3 rad)
{
	rad += Lradiance * dot(N, Li) * attenuation;
	rad = saturate(rad);
}

#endif