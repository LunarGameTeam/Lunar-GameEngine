#include "SharedCBuffer.hlsl"

Texture2D _MainTex : register(t0, MATERIAL_SPACE0);

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
BaseFragment VSMain(BaseVertex input) 
{
    BaseFragment output;     
    output.position = float4(input.position, 1);
    output.normal = float4(input.normal, 0);
    output.uv = input.uv;
    output.color = input.color;
	return output;
}
const static float correction = 1.0 / 2.2;

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float3 Uncharted2Tonemap(float3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float4 PSMain(BaseFragment input) : SV_TARGET
{
    float4 color = float4(0.0, 0.0, 0.0, 1.0);
    float3 hdrColor = _MainTex.Sample(_ClampSampler, input.uv).rgb;
    // reinhard tone mapping    
	// hdrColor = Uncharted2Tonemap(hdrColor * 4.5);
	// hdrColor = hdrColor * (1.0f / Uncharted2Tonemap((11.2f).xxx));    
    color.xyz = pow(hdrColor, correction.xxx); 
    return color;
}
