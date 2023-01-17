
/////////////
// GLOBALS //
/////////////


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
	[[vk::location(8)]] uint4 instancemessage : INSTANCEMESSAGE;	
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

struct PerObjectBuffer
{
	matrix worldMatrix;
};

cbuffer ViewBuffer : register(b1, space0)
{
	matrix cViewMatrix;
	matrix cProjectionMatrix;
	float2 cNearFar;
	float3 cCamPos;
};

struct PointLight
{
    float3 cLightPos;	
	float4 cLightColor;
	float cIndensity;
};

cbuffer SceneBuffer : register(b2, space0)
{
	PerObjectBuffer objectBuffers[128];
	float4 cDirectionLightColor;
    float3 cLightDirection;
	PointLight cPointLights[4];
};