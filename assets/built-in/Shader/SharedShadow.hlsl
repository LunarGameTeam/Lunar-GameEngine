#ifndef __SHARED_SHADOW__
#define __SHARED_SHADOW__

#include "SharedCBuffer.hlsl"
#include "SharedSampler.hlsl"


float GetShadowFactor(BaseFragment input)
{
	float shadow = 1.0;
	for(int i = 0; i < 6; i++)
	{
		float4 shadowCoord = mul(mul(float4(input.worldPosition.xyz, 1.0), cLightViewMatrix[i]), cLightProjMatrix[i]);
		shadowCoord = shadowCoord / shadowCoord.w;
		float3 uv = float3(shadowCoord.x, shadowCoord.y, 0);
		uv.x *= 0.5;
		uv.x += 0.5;
		uv.y *= 0.5;
		uv.y += 0.5;		
		uv.y = 1 - uv.y;
		uv.z = i;
		if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0  && uv.y < 1.0 && uv.y > 0.0 && uv.x < 1.0 && uv.x > 0.0)
		{
			float dist = _ShadowMap.Sample(SampleTypeClamp, uv).r;
			if (shadowCoord.w > 0.0 && dist < shadowCoord.z - 0.0001)
			{
				shadow = 0.0;
			}
		}
	}
	return shadow;
}
#endif