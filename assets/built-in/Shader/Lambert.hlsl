#include "SharedCBuffer.hlsl"
#include "SharedSampler.hlsl"
#include "SharedShadow.hlsl"
#include "VertexMain.hlsl"

#include "SharedLightingModel.hlsl"


Texture2D _NormalTex : register(t1, space2);

cbuffer MaterialBuffer : register(b3)
{
	float4 cDiffuseColor;
}


float4 PSMain(BaseFragment input) : SV_TARGET
{
    float4 color = float4(0,0,0,1);
	
	float3 Lo = normalize(cCamPos - input.worldPosition.xyz);

	float3 outColor = float3(0, 0, 0);

	float4 textureColor = _MainTex.Sample(SampleTypeClamp, input.uv);

	float shadow = GetShadowFactor(input);	
	{
		float3 Li = -cLightDirection;
		float3 lightColor = cDirectionLightColor;		
		float attenuation  = 1.0;
		LambertLighting(Lo, input.normal, Li, lightColor, textureColor.rgb, attenuation, outColor);    
	}

	for(int i = 0 ; i < cPointLightsCount; ++i)
	{
		float3 Li = normalize(cPointLights[i].cLightPos - input.worldPosition.xyz);
		float distance = length(input.worldPosition.xyz - cPointLights[i].cLightPos);
		float attenuation =  cPointLights[i].cIndensity / (1.0f + 0.09f * distance + 
    		    0.032f * (distance * distance));    
		float3 lightColor = cPointLights[i].cLightColor.xyz;
		lightColor = saturate(lightColor);
		LambertLighting(Lo, input.normal, Li, lightColor, textureColor.rgb, attenuation, outColor);
	}

	color = float4(shadow * saturate(outColor.xyz + cAmbientColor.xyz), 1.0);
    return color;
}