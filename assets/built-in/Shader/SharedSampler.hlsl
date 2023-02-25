#ifndef __SHARED_SAMPLER__
#define __SHARED_SAMPLER__

Texture2D _MainTex : register(t0, space2);

Texture2DArray _ShadowMap : register(t6, space2);
Texture2DArray _DirectionLightShadowMap : register(t7, space2);

SamplerState _ClampSampler : register(s0, space3);
SamplerState _RepeatSampler: register(s1, space3);

#endif