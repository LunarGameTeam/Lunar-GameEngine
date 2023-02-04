#ifndef __SHARED_SAMPLER__
#define __SHARED_SAMPLER__

Texture2D _MainTex : register(t3, space2);
Texture2D _ShadowMap[10] : register(t4, space2);
TextureCube _SkyTex : register(t5, space2);

SamplerState SampleTypeClamp : register(s6, space2);

// SamplerComparisonState SampleShadow : register(s5, space1);
// SamplerComparisonState SampleShadowPCF : register(s6, space1);
#endif