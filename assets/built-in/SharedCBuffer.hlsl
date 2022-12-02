
/////////////
// GLOBALS //
/////////////
struct PerObjectBuffer
{
	matrix worldMatrix;
};

cbuffer ObjectBuffer : register(b0, space0)
{
	PerObjectBuffer objectBuffers[128];
};

struct DirectionalLight
{

};

struct BaseVertex
{
    [[vk::location(0)]] float3 position : POSITION;
    [[vk::location(1)]] float4 color : TEXCOORD;
    [[vk::location(2)]] float3 normal : NORMAL;
    [[vk::location(3)]] float3 tangent : TANGENT;
	[[vk::location(4)]] float2 uv : TEXCOORD1;	
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

cbuffer ViewBuffer : register(b1, space0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	float2 nearFar;
	float3 camPos;
};

cbuffer SceneBuffer : register(b2, space0)
{
    float3 lightDirection;
	float4 diffuseColor;
	float4 ambientColor;
	DirectionalLight mainLight;
	
	float4 specColor;
	float3 csmSplits;
    matrix lightViewMatrix[4];
    matrix lightProjectionMatrix[4];
}