#define PI 3.14159265359f
#define MaxInstanceNum 100
//常量缓冲区
struct instance_data
{
	float4x4 world_matrix;
	uint4 animation_offset;
};
cbuffer per_instance : register(b0)
{
	instance_data _Instances[MaxInstanceNum];
}
cbuffer per_frame : register(b1)
{
	float4x4 view_matrix;
	float4x4 projectmatrix;
	float4x4 view_projectmatrix;
	float4x4 invview_matrix;
	float4   view_position;
}
//环境光IBL与brdf预计算
TextureCube environment_IBL_spec    : register(t0);
TextureCube environment_IBL_diffuse : register(t1);
texture2D   environment_brdf        : register(t2);
//顶点动画
struct mesh_anim
{
	float3 pos;
	float3 norm;
	float3 tangent;
};
StructuredBuffer<mesh_anim> input_point: register(t3);
//模型自带的贴图
texture2D   texture_model[]         : register(t4);
//静态采样器
SamplerState samTex_liner : register(s0);
//无动画顶点格式
struct VSInput
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	uint4  tex_id   : TEXID;
	float4 tex_uv   : TEXUV;
	uint InstanceId : SV_InstanceID;//instace索引号
};

//骨骼动画顶点格式
struct VSInputBone
{
	float3 position     : POSITION;
	float3 normal       : NORMAL;
	float3 tangent      : TANGENT;
	uint4  tex_id       : TEXID;
	float4 tex_uv       : TEXUV;
	uint4  bone_id      : BONEID;
	float4 bone_weight0 : BONEWEIGHTFIR;
	float4 bone_weight1 : BONEWEIGHTSEC;
	uint   InstanceId : SV_InstanceID;//实例ID号
	uint   VertexID   : SV_VertexID;  //顶点ID号
};

//顶点变形动画顶点格式
struct VSInputCatch
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	uint4  tex_id   : TEXID;
	float4 tex_uv   : TEXUV;
	uint4  anim_id  : ANIMID;
	uint InstanceId : SV_InstanceID;//instace索引号
};
struct PSInput
{
	float4 position : SV_POSITION;
	float4 pos_out  :POSITION;
	float3 normal   : NORMAL;
	float3 tangent  : TANGENT;
	uint4  tex_id   : TEXID;
	float4 tex_uv   : TEXUV;
};
struct PSInputNormal
{
	float4 position : SV_POSITION;
	float3 normal   : NORMAL;
};

PSInput VSMain(VSInput vinput)
{
	PSInput result;
	result.position = mul(float4(vinput.position, 1.0f), _Instances[vinput.InstanceId].world_matrix);
	result.position = mul(result.position, view_projectmatrix);
	result.pos_out = mul(float4(vinput.position, 1.0), _Instances[vinput.InstanceId].world_matrix);
	//模型不做不等缩放,可以使用世界变换作为法线变换
	result.normal = normalize(mul(float4(vinput.normal, 0.0), _Instances[vinput.InstanceId].world_matrix).xyz);
	result.tangent = normalize(mul(float4(vinput.tangent, 0.0), _Instances[vinput.InstanceId].world_matrix).xyz);
	result.tex_id = vinput.tex_id;
	result.tex_uv = vinput.tex_uv;
	return result;
}

PSInput VSMainBone(VSInputBone vinput)
{
	PSInput result;
	//根据cbuffer中的顶点动画偏移以及顶点的ID号获取当前顶点的动画数据位置
	uint index_anim = _Instances[vinput.InstanceId].animation_offset.x + vinput.VertexID;
	//获取当前顶点骨骼变换后的结果数据
	float3 used_position = input_point[index_anim].pos;
	float3 used_normal = normalize(input_point[index_anim].norm);
	float3 used_tangent = normalize(input_point[index_anim].tangent);
	//使用获取到的蒙皮数据进行几何变换
	result.position = mul(float4(used_position, 1.0f), _Instances[vinput.InstanceId].world_matrix);
	result.position = mul(result.position, view_projectmatrix);
	result.pos_out = mul(float4(used_position, 1.0), _Instances[vinput.InstanceId].world_matrix);
	result.normal = normalize(mul(float4(used_normal, 0.0), _Instances[vinput.InstanceId].world_matrix).xyz);
	result.tangent = normalize(mul(float4(used_tangent, 0.0), _Instances[vinput.InstanceId].world_matrix).xyz);
	result.tex_id = vinput.tex_id;
	result.tex_uv = vinput.tex_uv;
	return result;
}

PSInput VSMainPointCatch(VSInputCatch vinput)
{
}
//菲涅尔系数
float3 Fresnel_Schlick(float3 specularColor, float3 h, float3 v)
{
	return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}
float3 Fresnel_CookTorrance(float3 specularColor, float3 h, float3 v)
{
	float3 n = (1.0f + sqrt(specularColor)) / (1.0f - sqrt(specularColor));
	float c = saturate(dot(v, h));
	float3 g = sqrt(n * n + c * c - 1.0f);

	float3 part1 = (g - c) / (g + c);
	float3 part2 = ((g + c) * c - 1.0f) / ((g - c) * c + 1.0f);

	return max(0.0f.xxx, 0.5f * part1 * part1 * (1 + part2 * part2));
}
//法线扰乱系数
float NormalDistribution_GGX(float a, float NdH)
{
	// Isotropic ggx.
	float a2 = a * a;
	float NdH2 = NdH * NdH;

	float denominator = NdH2 * (a2 - 1.0f) + 1.0f;
	denominator *= denominator;
	denominator *= PI;

	return a2 / denominator;
}
float NormalDistribution_BlinnPhong(float a, float NdH)
{
	return (1 / (PI * a * a)) * pow(NdH, 2 / (a * a) - 2);
}
float NormalDistribution_Beckmann(float a, float NdH)
{
	float a2 = a * a;
	float NdH2 = NdH * NdH;

	return (1.0f / (PI * a2 * NdH2 * NdH2 + 0.001)) * exp((NdH2 - 1.0f) / (a2 * NdH2));
}
//几何体自遮挡系数
float Geometric_Implicit(float a, float NdV, float NdL)
{
	return NdL * NdV;
}
float Geometric_Neumann(float a, float NdV, float NdL)
{
	return (NdL * NdV) / max(NdL, NdV);
}
float Geometric_CookTorrance(float a, float NdV, float NdL, float NdH, float VdH)
{
	return min(1.0f, min((2.0f * NdH * NdV) / VdH, (2.0f * NdH * NdL) / VdH));
}
float Geometric_Kelemen(float a, float NdV, float NdL, float LdV)
{
	return (2 * NdL * NdV) / (1 + LdV);
}
float Geometric_Beckman(float a, float dotValue)
{
	float c = dotValue / (a * sqrt(1.0f - dotValue * dotValue));

	if (c >= 1.6f)
	{
		return 1.0f;
	}
	else
	{
		float c2 = c * c;
		return (3.535f * c + 2.181f * c2) / (1 + 2.276f * c + 2.577f * c2);
	}
}
float Geometric_Smith_Beckmann(float a, float NdV, float NdL)
{
	return Geometric_Beckman(a, NdV) * Geometric_Beckman(a, NdL);
}
float Geometric_GGX(float a, float dotValue)
{
	float a2 = a * a;
	return (2.0f * dotValue) / (dotValue + sqrt(a2 + ((1.0f - a2) * (dotValue * dotValue))));
}
float Geometric_Smith_GGX(float a, float NdV, float NdL)
{
	return Geometric_GGX(a, NdV) * Geometric_GGX(a, NdL);
}
float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
	// Smith schlick-GGX.
	float k = a * 0.5f;
	float GV = NdV / (NdV * (1 - k) + k);
	float GL = NdL / (NdL * (1 - k) + k);

	return GV * GL;
}
//计算pbr直接光照
float3 count_pbr_reflect(
	float3 light_color,
	float3 realAlbedo,
	float3 F0,
	float alpha,
	float3 light_dir_in,
	float3 normal,
	float3 direction_view,
	float diffuse_angle,
	float3 reflect_dir
)
{
	//将金属度及漫反射颜色还原为漫反射系数以及镜面反射系数
	/*
	float3 realAlbedo = tex_albedo_in.rgb - tex_albedo_in.rgb * tex_matallic;
	float3 F0 = lerp(0.03f, tex_albedo_in.rgb, tex_matallic);
	*/
	//半角方向
	float3 h_vec = normalize((light_dir_in + direction_view) / 2.0f);
	//漫反射强度(albedo的漫反射分量)
	float3 diffuse_out = realAlbedo / PI;
	//视线与法线的夹角
	float view_angle = max(0.03f, dot(direction_view, normal));
	//菲涅尔系数
	float3 fresnel = Fresnel_CookTorrance(F0, direction_view, h_vec);
	//NDF法线扰乱项
	//float alpha = max(0.001f, roughness * roughness);
	float nh_mul = saturate(dot(normal, h_vec));
	float ndf = NormalDistribution_GGX(alpha, nh_mul);
	/*
	float divide_ndf1 = nh_mul * nh_mul * (alpha * alpha - 1.0f) + 1.0f;
	float divide_ndf2 = PI * divide_ndf1 *divide_ndf1;
	float ndf = (alpha*alpha) / divide_ndf2;
	*/
	//GGX遮挡项
	float ggx = Geometric_Smith_Schlick_GGX(alpha, view_angle, diffuse_angle);
	/*
	float ggx_k = (tex_roughness + 1.0f) * (tex_roughness + 1.0f) / 8.0f;
	float ggx_v = view_angle / (view_angle*(1 - ggx_k) + ggx_k);
	float ggx_l = diffuse_angle / (diffuse_angle*(1 - ggx_k) + ggx_k);
	float ggx = ggx_v * ggx_l;
	float3 v = reflect(light_dir_in, normal);
	float blin_phong = pow(max(dot(v, direction_view), 0.0f), 10);
	*/
	//最终的镜面反射项
	float3 specular_out = (fresnel * ndf * ggx) / (4 * view_angle * diffuse_angle);
	return light_color * diffuse_angle * (diffuse_out * (1.0f - specular_out) + specular_out);
}
//计算pbr环境光照
float3 count_pbr_environment(
	float tex_roughness,
	float NoV,
	float3 reflect_dir,
	float3 realAlbedo,
	float3 F0,
	float tex_ao
)
{
	float4 EnvBRDF = environment_brdf.Sample(samTex_liner, float2(tex_roughness, NoV));
	uint index = tex_roughness * 6;
	float4 enviornment_color = environment_IBL_spec.SampleLevel(samTex_liner, reflect_dir, index);
	float4 color_diffuse = environment_IBL_diffuse.Sample(samTex_liner, reflect_dir);

	float3 specular_out = F0 * EnvBRDF.x + EnvBRDF.y;
	float3 diffuse_out = realAlbedo / PI;
	float3 ambient_diffuse = tex_ao * color_diffuse.xyz * (diffuse_out) * (1.0f - specular_out);
	float3 ambient_specular = tex_ao * enviornment_color.xyz * (specular_out);
	return ambient_specular + ambient_diffuse;
}
float4 PSMain(PSInput pin) : SV_TARGET
{
	//采样模型自带的纹理
	uint self_id_diffuse = pin.tex_id.x+0;     //漫反射贴图
	uint self_id_normal = pin.tex_id.x + 1; //法线贴图
	uint self_id_metallic = pin.tex_id.x + 3; //金属度贴图
	uint self_id_roughness = pin.tex_id.x + 4; //粗糙度贴图
	float4 diffuse_color = texture_model[self_id_diffuse].Sample(samTex_liner, pin.tex_uv.xy);
	float4 normal_color = texture_model[self_id_normal].Sample(samTex_liner, pin.tex_uv.xy);
	float4 metallic_color = texture_model[self_id_metallic].Sample(samTex_liner, pin.tex_uv.xy);
	float4 roughness_color = texture_model[self_id_roughness].Sample(samTex_liner, pin.tex_uv.xy);
	//法线变换计算真实的细节法线
	float3 N = pin.normal;
	float3 T = normalize(pin.tangent - N * pin.tangent * N);
	float3 B = cross(N, T);
	float3x3 T2W = float3x3(T, B, N);
	float3 real_normal = 2 * normal_color.rgb - 1;
	real_normal = normalize(mul(real_normal, T2W));
	//虚拟方向光光源
	float3 light_color = float3(1.0, 1.0, 1.0);
	float3 light_dir = normalize(float3(1,1,0));
	//计算直接光照所需的pbr参数
	float3 view_dir = normalize(view_position.xyz - pin.pos_out.xyz);
	float3 realAlbedo = diffuse_color.rgb - diffuse_color.rgb * metallic_color.r;
	float3 F0 = lerp(0.03f, diffuse_color.rgb, metallic_color.r);
	float alpha = roughness_color.r*roughness_color.r;
	float diffuse_angle = max(0.0f,dot(real_normal, light_dir));
	float3 reflect_dir = normalize(reflect(-view_dir, real_normal));
	float NoV = dot(real_normal, view_dir);
	float3 dir_light_color = max(0.0f, count_pbr_reflect(
		light_color,
		realAlbedo,
		F0,
		alpha,
		light_dir,
		real_normal,
		view_dir,
		diffuse_angle,
		reflect_dir
	));
	float3 environment_light_color = count_pbr_environment(
		roughness_color.r,
		NoV,
		reflect_dir,
		realAlbedo,
		F0,
		1.0f
	);
	return float4(dir_light_color + environment_light_color, diffuse_color.a);
}