#ifndef __SHARED_SHADOW__
#define __SHARED_SHADOW__

#include "SharedCBuffer.hlsl"
#include "SharedSampler.hlsl"


float GetShadowFactor(BaseFragment input)
{
	float shadow = 1.0;
	float4 shadowCoord = mul(mul(float4(input.worldPosition.xyz, 1.0), cLightViewMatrix[0]), cLightProjMatrix[0]);
	shadowCoord = shadowCoord / shadowCoord.w;
	shadowCoord.x *= 0.5;
	shadowCoord.x += 0.5;
	shadowCoord.y *= 0.5;
	shadowCoord.y += 0.5;
	shadowCoord.y = 1 - shadowCoord.y;
	
	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0  && shadowCoord.y < 1.0 && shadowCoord.y > 0.0 && shadowCoord.x < 1.0 && shadowCoord.x > 0.0)
	{
		float dist = _ShadowMap[0].Sample(SampleTypeClamp, shadowCoord.xy).r;
		if (shadowCoord.w > 0.0 && dist < shadowCoord.z - 0.007)
		{
			shadow = 0.0;
		}
	}
	return shadow;
}
#endif