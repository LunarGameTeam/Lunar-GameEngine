#ifndef __SHARED_SHADOW__
#define __SHARED_SHADOW__

#include "SharedCBuffer.hlsl"


float GetShadowFactor(float4 shadowCoord, float2 offset, int i)
{
	int2 texDim;
	uint elements;
	_ShadowMap.GetDimensions(texDim.x, texDim.y, elements);
	float scale = 1.5;
	float dx = scale * 1.0 / float(texDim.x);
	float dy = scale * 1.0 / float(texDim.y);
	offset.x *= dx;
	offset.y *= dy;
	float shadow = 1.0;
	float3 uv = float3(shadowCoord.x, shadowCoord.y, 0);
	uv *= 0.5;
	uv += 0.5;
	uv.y = 1 - uv.y;		
	uv.x += offset.x;
	uv.y += offset.y;
	uv.z = i;
	float dist = _ShadowMap.Sample(_ClampSampler, uv).r;
	if(shadowCoord.x > -1.0 && shadowCoord.x < 1.0 && shadowCoord.y > -1.0 && shadowCoord.y < 1.0 && shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
	{
		if (shadowCoord.w > 0.0 && dist < shadowCoord.z - 0.0003)
		{
			shadow = 0.0;
		}
	}
	
	return shadow;
}


float GetShadowPCF(float3 input)
{
//	float shadowFactor = 0.0;
//	int range = 1;
//	for(int i = 0; i < cShadowmapCount; i++)
//	{
//		float4 shadowCoord = mul(mul(float4(input, 1.0), cLightViewMatrix[i]), cLightProjMatrix[i]);
//		shadowCoord = shadowCoord / shadowCoord.w;
//		int count = 0;
//		shadowFactor = 0;
//		for (int x = -range; x <= range; x++)
//		{
//			for (int y = -range; y <= range; y++)
//			{
//				shadowFactor += GetShadowFactor(shadowCoord, float2(x, y), i);
//				count++;
//			}
//		}
//		shadowFactor = shadowFactor / count;
//		if(shadowFactor < 0.95)
//			return shadowFactor;		
//	}
	return 1.0;
}

#endif