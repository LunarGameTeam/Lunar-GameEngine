////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
};

cbuffer PassBuffer : register(b1)
{
	matrix _viewMatrix;
	matrix _projectionMatrix;
	float3 _camPos;
}

cbuffer LightBuffer : register(b2)
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
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct BaseFragment
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;	
    float4 lightViewPosition : TEXCOORD1;
	float3 viewDir : TEXCOORD2;
};




////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
BaseFragment VSMain(BaseVertex input)
{
    BaseFragment output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
	output.viewDir = normalize(_camPos - output.position.xyz);
    output.position = mul(output.position, _viewMatrix);
    output.position = mul(output.position, _projectionMatrix);
	
    
    
	// Calculate the position of the vertice as viewed by the light source.
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, _lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, _lightProjectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
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
Texture2D _MainTex : register(t0);
Texture2D _DepthTex : register(t1);


///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 PSMain(BaseFragment input) : SV_TARGET
{
    float bias;
    float4 color = float4(0,0,0,1);
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
    float diffuseLighting;
	float4 textureColor;
	float3 lightDir;
	// Invert the light direction.
    lightDir = -_lightDirection;
	float3 halfwayDir = input.viewDir + lightDir;
	halfwayDir = normalize(halfwayDir);
	float depth = input.position.z;
	float specLighting;
	// Set the bias value for fixing the floating point precision issues.
	bias = 0.01f;

	// Set the default output color to the ambient light value for all pixels.
   

	// Calculate the projected texture coordinates.
	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w * 0.5f + 0.5f;
	projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w * 0.5f + 0.5f;

		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
	depthValue = _DepthTex.Sample(SampleTypeWrap, projectTexCoord).r;
	lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		
		if(depthValue > lightDepthValue - 0.005)
		{	
			diffuseLighting = saturate(dot(input.normal, lightDir));
   			specLighting = pow(max(dot(halfwayDir, input.normal), 0), 4);
			color = _ambientColor +  _diffuseColor * diffuseLighting;
		}
		else
		{
			color = _ambientColor;
		}
	}
	else
    {

		diffuseLighting = saturate(dot(input.normal, lightDir));
   		specLighting = pow(max(dot(halfwayDir, input.normal), 0), 4);
		color = _ambientColor + _diffuseColor * diffuseLighting;
    }
	color = saturate(color);
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = _MainTex.Sample(SampleTypeClamp, input.tex);
	// Combine the light and texture color.
	color = color * textureColor;

    return color;
}