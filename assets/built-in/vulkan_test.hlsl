////////////////////////////////////////////////////////////////////////////////
// Filename: depth.vs
////////////////////////////////////////////////////////////////////////////////
#include "assets/built-in/SharedSampler.hlsl"

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0, space1)
{
	matrix _worldMatrix;
	float4 _color;
};

cbuffer PassBuffer : register(b1, space1)
{
	matrix _viewMatrix;
	matrix _projectionMatrix;
	float2 _nearFar;
	float3 _camPos;
}
cbuffer LightBuffer : register(b2, space1)
{
	float4 _ambientColor;
	float4 _specColor;
	float4 _diffuseColor;
    float3 _lightDirection;
    matrix _lightViewMatrix;
    matrix _lightProjectionMatrix;
}


//////////////
// TYPEDEFS //
//////////////
struct BaseVertex
{
[[vk::location(0)]]     float3 position : POSITION;
[[vk::location(1)]]     float2 uv : TEXCOORD0;
};
//////////////
// TYPEDEFS //
//////////////
struct BaseFragment
{
[[vk::location(0)]]    float4 position : SV_POSITION;
[[vk::location(1)]]    float2 uv: TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
BaseFragment VSMain(BaseVertex input) 
{
    BaseFragment output;
    output.position = float4(input.position, 1);
	output.uv = input.uv;
	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Filename: depth.ps
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMain(BaseFragment input) : SV_TARGET
{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
	float4 colorValue = _MainTex.Sample(SampleTypeClamp, input.uv);
	return colorValue * _color;
}
