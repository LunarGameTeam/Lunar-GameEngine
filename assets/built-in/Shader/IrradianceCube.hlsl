
#include "SharedCBuffer.hlsl"


#define NUM_SAMPLES 1024
const static float PI = 3.1415926536;
const static float deltaPhi = PI  * 2.0 / 180.0;
const static float deltaTheta =  PI * 0.5 / 64.0;


BaseFragment VSMain(BaseVertex input) 
{
    BaseFragment output;     
    output.position = float4(input.position, 1);
    output.position = mul(output.position, cViewMatrix);
   	output.position = mul(output.position, cProjectionMatrix);

    output.normal = float4(input.normal, 0);
	output.worldPosition = float4(input.position, 1);
    output.uv = input.uv;
    output.color = input.color;
	return output;
}

float4 PSMain(BaseFragment input) : SV_TARGET
{     
    float3 N = normalize(input.worldPosition.xyz);
	float3 up = float3(0.0, 1.0, 0.0);
	float3 right = normalize(cross(up, N));
	up = cross(N, right);

	const float TWO_PI = PI * 2.0;
	const float HALF_PI = PI * 0.5;

	float3 color = float3(0.0, 0.0, 0.0);
	uint sampleCount = 0u;
	for (float phi = 0.0; phi < TWO_PI; phi += deltaPhi) {
		for (float theta = 0.0; theta < HALF_PI; theta += deltaTheta) {
			float3 tempVec = cos(phi) * right + sin(phi) * up;
			float3 sampleVector = cos(theta) * N + sin(theta) * tempVec;
			color += _EnvTex.Sample(_ClampSampler, sampleVector).rgb * cos(theta) * sin(theta);
			sampleCount++;
		}
	}
	return float4(PI * color / float(sampleCount), 1.0);
}
