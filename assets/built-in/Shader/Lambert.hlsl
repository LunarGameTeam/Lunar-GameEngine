#include "SharedCBuffer.hlsl"
#include "SharedShadow.hlsl"
#include "VertexMain.hlsl"

#include "SharedLightingModel.hlsl"


Texture2D _MainTex : register(t0, MATERIAL_SPACE0);
Texture2D _NormalTex : register(t1, MATERIAL_SPACE0);
cbuffer MaterialBuffer : register(b2, MATERIAL_SPACE0)
{
	float4 cDiffuseColor;
}


float4 PSMain(BaseFragment input) : SV_TARGET
{
    float4 color = float4(0,0,0,1);	
	float3 V = normalize(cCamPos - input.worldPosition.xyz);
	float3 N = normalize(input.normal);
	float3 outColor = float3(0, 0, 0);

	float4 textureColor = _MainTex.Sample(_ClampSampler, input.uv);

	float shadow = GetShadowPCF(input.worldPosition.xyz);

	{
		float3 L = -cLightDirection;
		float3 lightColor = cDirectionLightColor;		
		float attenuation  = 1.0;
		LambertLighting(V, N, L, lightColor, textureColor.rgb, attenuation, outColor);    
	}

	for(int i = 0 ; i < cPointLightsCount; ++i)
	{
		float3 L = normalize(cPointLights[i].cLightPos - input.worldPosition.xyz);
		float distance = length(input.worldPosition.xyz - cPointLights[i].cLightPos);
		float attenuation =  cPointLights[i].cIndensity / (1.0f + 0.09f * distance + 
    		    0.032f * (distance * distance));    
		float3 lightColor = cPointLights[i].cLightColor.xyz;
		lightColor = saturate(lightColor);
		LambertLighting(V, N, L, lightColor, textureColor.rgb, attenuation, outColor);
	}

	color = float4(saturate(shadow * outColor.xyz + cAmbientColor.xyz), 1.0);
    return color;
}