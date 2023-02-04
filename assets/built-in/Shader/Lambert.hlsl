#include "SharedCBuffer.hlsl"
#include "SharedSampler.hlsl"
#include "SharedShadow.hlsl"
#include "VertexMain.hlsl"

#include "SharedLightingModel.hlsl"


cbuffer MaterialBuffer : register(b3)
{
	float4 cDiffuseColor;
}


float4 PSMain(BaseFragment input) : SV_TARGET
{
    float4 color = float4(0,0,0,1);
	// Invert the light direction.
	float3 Lo = normalize(cCamPos - input.worldPosition.xyz);
	float3 rad = float3(0, 0, 0);
	float4 textureColor = _MainTex.Sample(SampleTypeClamp, input.uv);
	
	float shadow = GetShadowFactor(input);
	if(shadow == 0.0)
		return float4(0,0,0,1);
	
	{
		float3 Li = -cLightDirection;
		LambertLighting(input, Lo,  input.normal, Li, cDirectionLightColor, 1.0, rad);    
	}

	for(int i = 0 ; i < cPointLightsCount; ++i)
	{
		float3 Li = normalize(cPointLights[i].cLightPos - input.worldPosition.xyz);
		float distance = length(input.worldPosition.xyz - cPointLights[i].cLightPos);
		float attenuation =  cPointLights[i].cIndensity / (1.0f + 0.09f * distance + 
    		    0.032f * (distance * distance));    
		float3 Lradiance = cPointLights[i].cLightColor.xyz;
		Lradiance = saturate(Lradiance);
		// CalcRadiance
		LambertLighting(input, Lo,  input.normal, Li, Lradiance, attenuation, rad);
	}
	// Combine the light and texture color.
	color = float4(rad.xyz, 1) * textureColor + cAmbientColor;
    return float4(rad.xyz, 1);
}