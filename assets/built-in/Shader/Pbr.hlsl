////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////
#include "assets/built-in/Shader/SharedCBuffer.hlsl"
#include "assets/built-in/Shader/SharedSampler.hlsl"
/////////////
// GLOBALS //
/////////////
static const float PI = 3.141592;
static const float Epsilon = 0.00001;

static const uint NumLights = 3;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

cbuffer MaterialBuffer : register(b3)
{
	float _Metallic;
	float _Smooth;
	float3 _AlebdoColor;
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
BaseFragment VSMain(BaseVertex input, uint inst : SV_InstanceID)
{
    BaseFragment output;
    uint instanceID = input.instancemessage.x;
	// Change the position vector to be 4 units for proper matrix calculations.
    float4 position = float4(input.position, 1.0);
	matrix worldMatrix = cRoWorldMatrix[instanceID];

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(position, worldMatrix);
	output.viewDir = normalize(cCamPos - output.position.xyz);
    output.position = mul(output.position, cViewMatrix);
	//output.depthLinear = (output.position.z - cNearFar.x)/ (cNearFar.y - cNearFar.x);
	output.depthLinear = 0;
   	output.position = mul(output.position, cProjectionMatrix);
	
	// Calculate the position of the vertice as viewed by the light source.
    output.worldPosition = mul(position, worldMatrix);

	// Store the texture coordinates for the pixel shader.
	output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
//////////////
// TEXTURES //
//////////////

///////////////////
// SAMPLE STATES //
///////////////////
 #define EPSILON 0.00001
float CalcShadowFactor(float lightDepthValue, float2 projectTexCoord, uint shadowMapIndex)
{
	float bias = 0.002;
	float shadowFactor = 0;
	float xOffset = 1 / 1024.0;
	float yOffset = 1 / 1024.0;
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1 ; j <=1; j++)
		{
			float2 offset = float2(xOffset * i, yOffset * j);
			float depthValue = _ShadowMap.Sample(SampleTypeClamp, float3(projectTexCoord + offset, shadowMapIndex)).r;			
			if(lightDepthValue < depthValue + bias)
				shadowFactor += 1;
		}
	}
	return shadowFactor / 9.0;
} 

float ndfGGX(float cosLh, float roughness)
{
	float alpha   = roughness * roughness;
	float alphaSq = alpha * alpha;

	float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
	return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float gaSchlickG1(float cosTheta, float k)
{
	return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
	return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// Shlick's approximation of the Fresnel factor.
float3 fresnelSchlick(float3 F0, float cosTheta)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void CalcRadiance(BaseFragment input, float3 Lo, float3 N, float3 Li, float3 Lradiance, out float3 rad)
{
	float3 albedo = _AlebdoColor;
	float metalness = _Metallic;
	float roughness = 0;//_Smooth;

	float cosLo = max(0.0, dot(N, Lo));
	
		
	// Specular reflection vector.
	float3 Lr = 2.0 * cosLo * N - Lo;

	// Fresnel reflectance at normal incidence (for metals use albedo color).
	float3 F0 = lerp(Fdielectric, albedo, metalness);

	// Direct lighting calculation for analytical lights.
	float3 directLighting = 0.0;

		// Half-vector between Li and Lo.
	float3 Lh = normalize(Li + Lo);

		// Calculate angles between surface normal and various light vectors.
	float cosLi = max(0.0, dot(N, Li));
	float cosLh = max(0.0, dot(N, Lh));

		// Calculate Fresnel term for direct lighting. 
	float3 F  = fresnelSchlick(F0, max(0.0, dot(Lh, Lo)));
		// Calculate normal distribution for specular BRDF.
	float D = ndfGGX(cosLh, roughness);
		// Calculate geometric attenuation for specular BRDF.
	float G = gaSchlickGGX(cosLi, cosLo, roughness);

		// Diffuse scattering happens due to light being refracted multiple times by a dielectric medium.
		// Metals on the other hand either reflect or absorb energy, so diffuse contribution is always zero.
		// To be energy conserving we must scale diffuse BRDF contribution based on Fresnel factor & metalness.
	float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);

		// Lambert diffuse BRDF.
		// We don't scale by 1/PI for lighting & material units to be more convenient.
		// See: https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
	float3 diffuseBRDF = kd * albedo;

		// Cook-Torrance specular microfacet BRDF.
	float3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * cosLo);

		// Total contribution for this light.
	directLighting += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;

	rad += directLighting;
}

float4 PSMain(BaseFragment input) : SV_TARGET
{
// Sample input textures to get shading model params.
	float3 albedo = _AlebdoColor;
	float metalness = _Metallic;
	float roughness = 0;//_Smooth;

	// Outgoing light direction (vector from world-space fragment position to the "eye").
	float3 Lo = normalize(cCamPos - input.worldPosition.xyz);

	float4 lightViewPosition = input.worldPosition;

	//先屏蔽CSM功能
	// uint csm_index = 0;
	// float z = input.depthLinear;
	// if(z <= csmSplits.x)
	// 	csm_index = 0;
	// else if(z <= csmSplits.y)
	// 	csm_index = 1;
	// else if(z <= csmSplits.z)
	// 	csm_index = 2;
	// else
	// 	csm_index = 3;
	// lightViewPosition = mul(lightViewPosition, lightViewMatrix[csm_index]);
    // lightViewPosition = mul(lightViewPosition, lightProjectionMatrix[csm_index]);	
	
    float2 uv2 = float2(lightViewPosition.x * 0.5 / lightViewPosition.w + 0.5, -lightViewPosition.y * 0.5 / lightViewPosition.w + 0.5);
	// Get current fragment's normal and transform to world space.
	//float shadowFactor = CalcShadowFactor(lightViewPosition.z / lightViewPosition.w, uv2, csm_index);
	
	float3 rad = float3(0, 0, 0);
	// Direction Light
	{
		float3 Li = -cLightDirection;
		float3 Lradiance = cDirectionLightColor.xyz;
		// CalcRadiance
		CalcRadiance(input, Lo,  input.normal, Li, Lradiance, rad);
	}

	// Point Lights
	for(int i = 0 ; i < cPointLightsCount; ++i)
	{
		float3 Li = normalize(input.worldPosition.xyz - cPointLights[i].cLightPos);
		float distance = length(input.worldPosition.xyz - cPointLights[i].cLightPos);
		float attenuation =  cPointLights[i].cIndensity / (1.0f + 0.09f * distance + 
    		    0.032f * (distance * distance));    
		float3 Lradiance = cPointLights[i].cLightColor.xyz * attenuation;
		// CalcRadiance
		CalcRadiance(input, Lo,  input.normal, Li, Lradiance, rad);
	}

	// if(dot(input.normal, - cLightDirection) > 0)
	//	return float4((float(csm_index + 1) / 4.0f * shadowFactor) * rad, 1);
	// Final fragment color
	
	float4 color = _MainTex.Sample(SampleTypeClamp, input.uv);
	//return float4(rad, 1);
	return float4(rad, 1.0);
}