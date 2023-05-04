cbuffer MaterialConstants : register(b0)
{
	float4 BaseColorFactor;
	float3 EmissiveFactor;
	float MetallicFactor;

	float RoughnessFactor;
	float NormalScale;
	float OcclusionStrength;
	bool HasBaseColorTex;

	bool HasMetallicRoughnessTex;
	bool HasNormalTex;
	bool HasOcclusionTex;
	bool HasEmissiveTex;
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

static const float PI = 3.141592;
static const float Epsilon = 0.00001;

struct Light
{
	float3 pos;
	float3 dir;
	float3 color;
};

const static Light g_light = { {0.0, 2.0, 0.0}, {0.0, -1.0, 0.0}, {1.0, 1.0, 1.0} };

struct PSInput
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	float2 uv : TEXCOORD0;
};

float3 GetNormal(PSInput psInput)
{
	float3 n = psInput.normal;
	
	if (HasNormalTex)
	{
		float du1 = ddx(psInput.uv).x;
		float dv1 = ddx(psInput.uv).y;
		float du2 = ddy(psInput.uv).x;
		float dv2 = ddy(psInput.uv).y;

		float3 dp1 = ddx(psInput.worldPos);
		float3 dp2 = ddy(psInput.worldPos);

		// t := tangent, b := bitangent
		// dp1 = du1 * t + dv1 * b
		// dp2 = du2 * t + dv2 * b
		// => system of equations, solve for t.
		// b = (dp1 - du1 * t) / dv1
		// dp2 = du2 * t + dv2 * (dp1 - du1 * t) / dv1
		// dp2 = du2 * t + dv2 * dp1 / dv1 - dv2 * du1 * t / dv1
		// dp2 - dv2 * dp1 / dv1 = (du2 - dv2 * du1 / dv1) * t
		// dv1 * dp2 - dv2 * dp1 = (dv1 * du2 - dv2 * du1) * t
		// t = (dv1 * dp2 - dv2 * dp1) / (dv1 * du2 - dv2 * du1)
		float3 t = (dv1 * dp2 - dv2 * dp1) / (dv1 * du2 - dv2 * du1);

		// gram schmidt orthogonalization
		t = normalize(t - n * dot(n, t));

		// b = (dp1 - du1 * t) / dv1
		// you can solve for b, too, but there is more proper way.
		float3 b = cross(n, t);

		float3x3 tbn = float3x3(t, b, n); // row major matrix, so we can multiply by row vectors

		n = NormalTex.Sample(Sampler_NormalTex, psInput.uv).xyz;
		n = n * 2.0 - 1.0; // normalize to [-1, 1]

		n = normalize(n * float3(NormalScale, NormalScale, 1.0));
		n = mul(n, tbn); // tbn is row major, so we can multiply by row vectors
	}

	return n;
}


float4 main(PSInput psInput) : SV_Target0
{
	float3 n = GetNormal(psInput);

	float intensity = dot(-g_light.dir, n);

	float4 color = (HasBaseColorTex) ? BaseColorTex.Sample(Sampler_BaseColorTex, psInput.uv) : float4(1.0, 1.0, 1.0, 1.0);
	color *= BaseColorFactor * intensity;

	if (HasOcclusionTex)
	{
		float ao = OcclusionTex.Sample(Sampler_OcclusionTex, psInput.uv).r;
		color += color * OcclusionStrength * (ao - 1.0); //color = lerp(color, color * ao, OcclusionStrength);
	}

	if (HasEmissiveTex)
	{
		float3 emissive = EmissiveTex.Sample(Sampler_EmissiveTex, psInput.uv).rgb * EmissiveFactor;
		color += float4(emissive, 0.0);
	}

	return color;

}