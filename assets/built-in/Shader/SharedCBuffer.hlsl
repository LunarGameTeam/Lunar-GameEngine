#ifndef __SHARED_CBUFFER__
#define __SHARED_CBUFFER__

#define SCENE_SPACE0 space0
#define SCENE_SPACE1 space1

#define VIEW_SPACE0 space2
#define VIEW_SPACE1 space3

#define MATERIAL_SPACE0 space4
#define MATERIAL_SPACE1 space5

#define SKIN_SPACE0 space6

#ifndef USE_SKIN_VERTEX
#define USE_SKIN_VERTEX 0
#endif
// Static Samplers


struct BaseVertex
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float4 color : TEXCOORD;
    [[vk::location(2)]] float3 normal : NORMAL;
    [[vk::location(3)]] float3 tangent : TANGENT;
	[[vk::location(4)]] float2 uv : TEXCOORD1;
	[[vk::location(5)]] float2 uv2 : TEXCOORD2;
	[[vk::location(6)]] float2 uv3 : TEXCOORD3;
	[[vk::location(7)]] float2 uv4 : TEXCOORD4;
#if USE_SKIN_VERTEX
	[[vk::location(8)]] uint4 blendindex : BLENDINDEX;
    [[vk::location(9)]] uint4 blendweight : BLENDWEIGHT;
#endif
	[[vk::location(10)]] uint4 instancemessage : INSTANCEMESSAGE;	
};

struct BaseFragment
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
	float3 normal : NORMAL;	
    float4 worldPosition : TEXCOORD1;
	float depthLinear : POSITION1;
    float4 color : COLOR0;
	float3 viewDir : TEXCOORD2;
};

//
// View Update Frequence
// 每次Viwe更新的Buffer
//
cbuffer ViewBuffer : register(b0, VIEW_SPACE0)
{
	matrix cViewMatrix;
	matrix cProjectionMatrix;
	float2 cNearFar;
	float3 cCamPos;
};

#if USE_SKIN_VERTEX
StructuredBuffer<float4x4> SkinMatrixBuffer : register(t0, SKIN_SPACE0);
#endif

Texture2DArray _ShadowMap : register(t1, VIEW_SPACE0);
Texture2DArray _DirectionLightShadowMap : register(t2, VIEW_SPACE0);
StructuredBuffer<float4x4> RoWorldMatrixBuffer : register(t3, VIEW_SPACE0);
//
// Scene Update Frequence
// Scene 更新的 Buffer 和贴图
//

cbuffer PointBasedLightParameter : register(b0, SCENE_SPACE0)
{
	uint4 PointBasedLightNum;

	//float4 cAmbientColor;
	//Direction Light
	//float4 cDirectionLightColor;	
    //float3 cLightDirection;
	//float  cDirectionLightIndensity;
	//Cacsde Direction Light Matrix
	//matrix cDirectionLightViewMatrix[4];
	//matrix cDirectionLightProjMatrix[4];
	
	//Point Light Matrix
	//PointLight cPointLights[4];
	//int cPointLightsCount;
	//matrix cLightViewMatrix[6];
	//matrix cLightProjMatrix[6];
	//int cShadowmapCount;
	
};
StructuredBuffer<float4> PointBasedLightDataBUffer : register(t4, VIEW_SPACE0);

//天空盒贴图
TextureCube _EnvTex : register(t1, SCENE_SPACE0);
TextureCube _IrradianceTex : register(t2, SCENE_SPACE0);
SamplerState _ClampSampler : register(s3, SCENE_SPACE0);
SamplerState _RepeatSampler: register(s4, SCENE_SPACE0);

#endif