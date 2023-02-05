#ifndef __SHARED_SAMPLER__
#define __SHARED_SAMPLER__

Texture2D _MainTex : register(t0, space2);
Texture2DArray _ShadowMap : register(t1, space2);
Texture2DArray _DirectionLightShadowMap : register(t2, space2);

SamplerState SampleTypeClamp : register(s1, space3);

// SamplerComparisonState SampleShadow : register(s5, space1);
// SamplerComparisonState SampleShadowPCF : register(s6, space1);
#endif