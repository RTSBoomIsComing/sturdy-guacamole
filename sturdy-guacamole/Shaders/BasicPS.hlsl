cbuffer MaterialConstants : register(b0)
{
	float4 BaseColorFactor;
	float3 EmissiveFactor;
	float MetallicFactor;

	float RoughnessFactor;
	bool HasBaseColorTex;
	bool HasMetallicRoughnessTex;
	bool HasNormalTex;

	bool HasOcclusionTex;
	bool HasEmissiveTex;
	bool dummy[2];
};

cbuffer CommonConstants : register(b1)
{
	float4x4 ViewMatrix;		// World to view
	float4x4 ViewInv;			// View to world
	float4x4 ProjMatrix;		// View to NDC
	float4x4 ProjInv;			// NDC to view
	float4x4 ViewProjMatrix;	// World to NDC
	float4x4 ViewProjInv;		// NDC to world

	float3 ViewerPos;			// World space viewer position
};

Texture2D<float4> BaseColorTex : register(t0);
Texture2D<float4> MetallicRoughnessTex : register(t1);
Texture2D<float4> NormalTex : register(t2);
Texture2D<float4> OcclusionTex : register(t3);
Texture2D<float4> EmissiveTex : register(t4);

SamplerState Sampler_BaseColorTex  : register(s0);
SamplerState Sampler_MetallicRoughnessTex  : register(s1);
SamplerState Sampler_NormalTex  : register(s2);
SamplerState Sampler_OcclusionTex  : register(s3);
SamplerState Sampler_EmissiveTex  : register(s4);

struct VSOutput
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	float2 uv : TEXCOORD0;
};

float4 main(VSOutput vsOutput) : SV_Target0
{
	return BaseColorTex.Sample(Sampler_BaseColorTex, vsOutput.uv);

}