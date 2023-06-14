////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////
#include "SharedCBuffer.hlsl"
#include "SharedShadow.hlsl"
/////////////
// GLOBALS //
/////////////
static const float PI = 3.141592;
static const float Epsilon = 0.00001;

static const uint NumLights = 3;

// Constant normal incidence Fresnel factor for all dielectrics.
static const float3 Fdielectric = 0.04;

Texture2D _MainTex : register(t0, MATERIAL_SPACE0);
Texture2D _LUTTex : register(t2, MATERIAL_SPACE0);

cbuffer MaterialBuffer : register(b3, MATERIAL_SPACE0)
{
	float _Metallic;
	float _Roughness;
	float3 _AlebdoColor;
}

void SplitUint32ToUint16(in uint4 data, out uint4 a, out uint4 b)
{
	a.x = (data.x >> 16) & 0xFFFF;
	a.y = data.x & 0xFFFF;
	a.z = (data.y >> 16) & 0xFFFF;
	a.w = data.y & 0xFFFF;

	b.x = (data.z >> 16) & 0xFFFF;
	b.y = data.z & 0xFFFF;
	b.z = (data.w >> 16) & 0xFFFF;
	b.w = data.w & 0xFFFF;
}

void SplitUint32ToNFloat16(in uint4 data, out float4 a, out float4 b)
{
	a.x = (float)((data.x >> 16) & 0xFFFF);
	a.y = (float)(data.x & 0xFFFF);
	a.z = (float)((data.y >> 16) & 0xFFFF);
	a.w = (float)(data.y & 0xFFFF);

	b.x = (float)((data.z >> 16) & 0xFFFF);
	b.y = (float)(data.z & 0xFFFF);
	b.z = (float)((data.w >> 16) & 0xFFFF);
	b.w = (float)(data.w & 0xFFFF);

	a = a / 65535.0f;
	b = b / 65535.0f;
}
#if USE_SKIN_VERTEX
void TransformSkinVertex(inout float4 position,inout float3 normal,inout float3 tangent,uint4 ref0,uint4 ref1,float4 weight0,float4 weight1)
{
	float4 positonLegency = position;
	position = float4(0.0f, 0.0f, 0.0f,1.0f);
	float4 normalLegency = float4(normal,0.0f);
	normal = float3(0.0f, 0.0f, 0.0f);
	float4 tangentLegency = float4(tangent,0.0f);
	tangent = float3(0.0f, 0.0f, 0.0f);

	position.xyz += mul(positonLegency, SkinMatrixBuffer[ref0.x]).xyz * weight0.x;
	normal += mul(normalLegency, SkinMatrixBuffer[ref0.x]).xyz * weight0.x;
	tangent += mul(tangentLegency, SkinMatrixBuffer[ref0.x]).xyz * weight0.x;

	position.xyz += mul(positonLegency, SkinMatrixBuffer[ref0.y]).xyz * weight0.y;
	normal += mul(normalLegency, SkinMatrixBuffer[ref0.y]).xyz * weight0.y;
	tangent += mul(tangentLegency, SkinMatrixBuffer[ref0.y]).xyz * weight0.y;

	position.xyz += mul(positonLegency, SkinMatrixBuffer[ref0.z]).xyz * weight0.z;
	normal += mul(normalLegency, SkinMatrixBuffer[ref0.z]).xyz * weight0.z;
	tangent += mul(tangentLegency, SkinMatrixBuffer[ref0.z]).xyz * weight0.z;

	position.xyz += mul(positonLegency, SkinMatrixBuffer[ref0.w]).xyz * weight0.w;
	normal += mul(normalLegency, SkinMatrixBuffer[ref0.w]).xyz * weight0.w;
	tangent += mul(tangentLegency, SkinMatrixBuffer[ref0.w]).xyz * weight0.w;
	
	position.xyz += mul(positonLegency, SkinMatrixBuffer[ref1.x]).xyz * weight1.x;
	normal += mul(normalLegency, SkinMatrixBuffer[ref1.x]).xyz * weight1.x;
	tangent += mul(tangentLegency, SkinMatrixBuffer[ref1.x]).xyz * weight1.x;

	position.xyz += mul(positonLegency, SkinMatrixBuffer[ref1.y]).xyz * weight1.y;
	normal += mul(normalLegency, SkinMatrixBuffer[ref1.y]).xyz * weight1.y;
	tangent += mul(tangentLegency, SkinMatrixBuffer[ref1.y]).xyz * weight1.y;

	position.xyz += mul(positonLegency, SkinMatrixBuffer[ref1.z]).xyz * weight1.z;
	normal += mul(normalLegency, SkinMatrixBuffer[ref1.z]).xyz * weight1.z;
	tangent += mul(tangentLegency, SkinMatrixBuffer[ref1.z]).xyz * weight1.z;

	position.xyz += mul(positonLegency, SkinMatrixBuffer[ref1.w]).xyz * weight1.w;
	normal += mul(normalLegency, SkinMatrixBuffer[ref1.w]).xyz * weight1.w;
	tangent += mul(tangentLegency, SkinMatrixBuffer[ref1.w]).xyz * weight1.w;
}
#endif
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
BaseFragment VSMain(BaseVertex input, uint inst : SV_InstanceID)
{
    BaseFragment output;
    uint instanceID = input.instancemessage.x;
	// Change the position vector to be 4 units for proper matrix calculations.
    float4 position = float4(input.position, 1.0);
	float3 normal = input.normal;
	float3 tangent = input.tangent;
	matrix worldMatrix = cRoWorldMatrix[instanceID];
#if USE_SKIN_VERTEX
	uint4 blendIndexA,blendIndexB;
	float4 blendweightA,blendweightB;
	SplitUint32ToUint16(input.blendindex,blendIndexA,blendIndexB);
	SplitUint32ToNFloat16(input.blendweight, blendweightA,blendweightB);
	TransformSkinVertex(position,normal,tangent,blendIndexA,blendIndexB,blendweightA,blendweightB);
#endif
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
    output.normal = mul(normal, (float3x3)worldMatrix);
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

//#define ROUGHNESS_PATTERN 1

float3 materialcolor()
{
	return _AlebdoColor;
}

// Normal Distribution function --------------------------------------
float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/(PI * denom*denom);
}

// Geometric Shadowing function --------------------------------------
float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	float GL = dotNL / (dotNL * (1.0 - k) + k);
	float GV = dotNV / (dotNV * (1.0 - k) + k);
	return GL * GV;
}

// Fresnel function ----------------------------------------------------
float3 F_Schlick(float cosTheta, float3 F0)
{
	float3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
	return F;
}
float3 F_SchlickR(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max((1.0 - roughness).xxx, F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 prefilteredReflection(float3 R, float roughness)
{
	const float MAX_REFLECTION_LOD = 9.0; // todo: param/const
	float lod = roughness * MAX_REFLECTION_LOD;
	float lodf = floor(lod);
	float lodc = ceil(lod);
	float3 a = _EnvTex.Sample(_ClampSampler, R).rgb;
	float3 b = _EnvTex.Sample(_ClampSampler, R).rgb;
	return lerp(a, b, lod - lodf);
}

// Specular BRDF composition --------------------------------------------

float3 BRDF(float3 L, float3 V, float3 N,float3 lightColor, float metallic, float roughness)
{
	// Precalculate vectors and dot products
	float3 H = normalize (V + L);
	float dotNV = clamp(dot(N, V), 0.0, 1.0);
	float dotNL = clamp(dot(N, L), 0.0, 1.0);
	float dotLH = clamp(dot(L, H), 0.0, 1.0);
	float dotNH = clamp(dot(N, H), 0.0, 1.0);

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, materialcolor(), metallic); // * material.specular

	float3 color = float3(0.0, 0.0, 0.0);

	if (dotNL > 0.0)
	{
		float rroughness = max(0.05, roughness);
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		float3 F = F_Schlick(dotNV, F0);

		float3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);
		float3 kD = (float3(1.0, 1.0, 1.0) - F) * (1.0 - metallic);
		color += (kD * _AlebdoColor / PI + spec) * dotNL;
	}

	return color;
}



float4 PSMain(BaseFragment input) : SV_TARGET
{
// Sample input textures to get shading model params.
	float3 albedo = _AlebdoColor;
	float metallic = _Metallic;
	float roughness = _Roughness;

	float3 V = normalize(cCamPos - input.worldPosition.xyz);
	float3 N = normalize(input.normal);
	float3 R = reflect(-V, N);

	float4 lightViewPosition = input.worldPosition;

	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, _AlebdoColor, _Metallic);

	float shadow = GetShadowPCF(input.worldPosition.xyz);

	float3 Lo = float3(0, 0, 0);
	// Direction Light
	{
		float3 L = normalize(-cLightDirection);
		float3 lightColor = cDirectionLightColor.xyz;
		// CalcRadiance
		// Lo+= BRDF(L, V,  N, lightColor, metallic, roughness);
	}

	// Point Lights
	for(int i = 0 ; i < cPointLightsCount; ++i)
	{
		float3 L = normalize(cPointLights[i].cLightPos- input.worldPosition.xyz);
		float distance = length(input.worldPosition.xyz - cPointLights[i].cLightPos);
		float attenuation =  cPointLights[i].cIndensity / (1.0f + 0.09f * distance + 
    		    0.032f * (distance * distance));    
		float3 lightColor = cPointLights[i].cLightColor.xyz * attenuation;
		// CalcRadiance
		Lo+= BRDF(L, V,  N, lightColor, metallic, roughness);
	}
		
	float2 brdf = _LUTTex.Sample(_ClampSampler, float2(max(dot(N, V), 0.0), roughness)).rg;
	float3 reflection = prefilteredReflection(R, roughness).rgb;
	float3 irradiance = _IrradianceTex.Sample(_ClampSampler, N).rgb;

	// Diffuse based on irradiance
	float3 diffuse = irradiance * albedo;

	float3 F = F_SchlickR(max(dot(N, V), 0.0), F0, roughness);

	// Specular reflectance
	float3 specular = reflection * (F * brdf.x + brdf.y);
	
	// Ambient part
	float3 kD = 1.0 - F;
	kD *= 1.0 - metallic;
	float3 ambient = (kD * diffuse + specular);

	float3 color =  Lo + ambient;
	return float4(color * shadow, 1.0);
}