#include "assets/built-in/Shader/SharedCBuffer.hlsl"
#include "assets/built-in/Shader/SharedSampler.hlsl"

cbuffer MaterialBuffer : register(b3)
{
	float4 cSpecularColor;
	float4 cDiffuseColor;
}

#include "assets/built-in/Shader/VertexMain.hlsl"

void CalcRadiance(BaseFragment input, float3 Lo, float3 N, float3 Li, float3 Lradiance, float attenuation, out float3 rad)
{
	float cosAngIncidence = dot(N, Li);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);	
	
	float3 halfAngle = normalize(Li + Lo);
	float blinnTerm = dot(N, halfAngle);
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, 2);
	rad += Lradiance * cosAngIncidence * attenuation+ (cSpecularColor.xyz * attenuation * blinnTerm);
	rad = saturate(rad);
}

float4 PSMain(BaseFragment input) : SV_TARGET
{
    float4 color = float4(0,0,0,1);
	// Invert the light direction.
	float3 Lo = normalize(cCamPos - input.worldPosition.xyz);
	float3 rad = float3(0,0,0);
	CalcRadiance(input, Lo,  input.normal, -cLightDirection, cDirectionLightColor, 1.0, rad);    
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	float4 textureColor = _MainTex.Sample(SampleTypeClamp, input.uv);

	for(int i = 0 ; i < cPointLightsCount; ++i)
	{
		float3 Li = normalize(cPointLights[i].cLightPos - input.worldPosition.xyz);
		float distance = length(input.worldPosition.xyz - cPointLights[i].cLightPos);
		float attenuation =  cPointLights[i].cIndensity / (1.0f + 0.09f * distance + 
    		    0.032f * (distance * distance));    
		float3 Lradiance = cPointLights[i].cLightColor.xyz;
		Lradiance = saturate(Lradiance);
		// CalcRadiance
		CalcRadiance(input, Lo,  input.normal, Li, Lradiance, attenuation, rad);
	}
	// Combine the light and texture color.
	color = float4(rad.xyz, 1) * textureColor;
    return float4(rad.xyz, 1);
}