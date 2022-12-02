#define PI 3.141592653

//http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
unsigned int ReverseBits32(unsigned int bits)
{
	bits = (bits << 16) | (bits >> 16);
	bits = ((bits & 0x00ff00ff) << 8) | ((bits & 0xff00ff00) >> 8);
	bits = ((bits & 0x0f0f0f0f) << 4) | ((bits & 0xf0f0f0f0) >> 4);
	bits = ((bits & 0x33333333) << 2) | ((bits & 0xcccccccc) >> 2);
	bits = ((bits & 0x55555555) << 1) | ((bits & 0xaaaaaaaa) >> 1);
	return bits;
}
float2 Hammersley(int Index, int NumSamples)
{
	float t1 = 0.00125125889;
	float E1 = 1.0 * Index / NumSamples + t1;
	E1 = E1 - int(E1);
	float E2 = float(ReverseBits32(Index)) * 2.3283064365386963e-10;
	return float2(E1, E2);
}
float3 ImportanceSampleGGX(float2 E, float Roughness)
{
	float m = Roughness * Roughness;
	float m2 = m * m;

	float Phi = 2 * PI * E.x;
	float CosTheta = sqrt((1 - E.y) / (1 + (m2 - 1) * E.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	float3 H = float3(SinTheta * cos(Phi), SinTheta * sin(Phi), CosTheta);

	float d = (CosTheta * m2 - CosTheta) * CosTheta + 1;
	float D = m2 / (PI*d*d);
	float PDF = D * CosTheta;

	return H;
}
float Vis_SmithJointApprox(float Roughness, float NoV, float NoL)
{
	float a = Roughness * Roughness;
	float Vis_SmithV = NoL * (NoV * (1 - a) + a);
	float Vis_SmithL = NoV * (NoL * (1 - a) + a);
	return 0.5 / (Vis_SmithV + Vis_SmithL);
}
float2 IntegrateBRDF(float Roughness, float NoV)
{
	if (Roughness < 0.04) Roughness = 0.04;

	float3 V = float3(sqrt(1 - NoV * NoV), 0, NoV);
	float A = 0, B = 0;
	int NumSamples = 1024;
	for (int i = 0; i < NumSamples; i++)
	{
		float2 E = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(E, Roughness);
		float3 L = 2 * dot(V, H) * H - V;

		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));

		if (NoL > 0)
		{
			float Vis = Vis_SmithJointApprox(Roughness, NoV, NoL);

			float a = Roughness * Roughness;
			float a2 = a * a;
			float Vis_SmithV = NoL * sqrt(NoV * (NoV - NoV * a2) + a2);
			float Vis_SmithL = NoV * sqrt(NoL * (NoL - NoL * a2) + a2);

			float NoL_Vis_PDF = NoL * Vis * (4 * VoH / NoH);

			float Fc = pow(1 - VoH, 5);
			A += (1 - Fc) * NoL_Vis_PDF;
			B += Fc * NoL_Vis_PDF;
		}
	}
	float2 res = float2(A, B);
	res /= NumSamples;
	return res;
}

struct VertexIn
{
	float4	pos 	: POSITION;     //����λ��
	float4  tex1    : TEXCOORD;     //������������
};
struct VertexOut
{
	float4 PosH       : SV_POSITION; //��Ⱦ���߱�Ҫ����
	float2 Tex        : TEXCOORD1;   //��������
};
VertexOut VSMain(VertexIn vin)
{
	VertexOut vout;
	vout.PosH = vin.pos;
	vout.Tex = vin.tex1.xy;
	return vout;
}
float4 PSMain(VertexOut pin) : SV_Target
{
	float4 texcolor = float4(IntegrateBRDF(pin.Tex.x,pin.Tex.y),0.0f,0.0f);
	return texcolor;
}