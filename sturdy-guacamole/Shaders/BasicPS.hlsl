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
static const float EPSILON = 0.00001;

struct Light
{
	float3 pos;
	float3 dir;
	float3 color;
};

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
		n = n * 2.0 - float3(1.0, 1.0, 1.0); // normalize to [-1, 1]

		n = mul(n, tbn); // tbn is row major, so we can multiply by row vectors
		n = normalize(n * float3(NormalScale, NormalScale, 1.0));
	}

	return n;
}

struct PBRInput
{
	float alpha;
	float NdotL;
	float NdotV;
	float NdotH;
	float HdotL;
	float HdotV;
};

float3 Fresnel_schlick(float3 f0, float VdotH)
{
	return f0 + (1.0 - f0) * pow(1.0 - abs(VdotH), 5.0);
}

// The separable form of the Smith joint masking - shadowing function
float G_Smith(PBRInput pbrInput)
{
	const float a2 = pbrInput.alpha * pbrInput.alpha;
	const float NdotL = pbrInput.NdotL;
	const float NdotV = pbrInput.NdotV;
	const float HdotL = pbrInput.HdotL;
	const float HdotV = pbrInput.HdotV;

	const float shadowing = 2.0 * abs(NdotL) * (HdotL > 0.0) / max(abs(NdotL) + sqrt(a2 + (1.0 - a2) * (NdotL * NdotL)), EPSILON);
	const float masking = 2.0 * abs(NdotV) * (HdotV > 0.0) / max(abs(NdotV) + sqrt(a2 + (1.0 - a2) * (NdotV * NdotV)), EPSILON);
	return shadowing * masking;
}

// The Trowbridge-Reitz/GGX microfacet distribution
float D_GGX(PBRInput pbrInput)
{
	const float alpha = pbrInput.alpha;
	const float NdotH = pbrInput.NdotH;

	const float f = (NdotH * NdotH * (alpha * alpha - 1.0) + 1.0);
	return alpha * alpha * (NdotH > 0.0) / max((PI * f * f), EPSILON);
}

// The specular reflection is a microfacet BRDF
// microfacet BRDF = G * D / (4 * NdotL * NdotV)
float3 Specular_brdf(PBRInput pbrInput)
{
	return G_Smith(pbrInput) * D_GGX(pbrInput) / max(4.0 * abs(pbrInput.NdotL) * abs(pbrInput.NdotV), EPSILON);
}

// The diffuse reflection is a Lambertian BRDF
float3 Diffuse_brdf(float3 color)
{
	return color / PI;
}

float4 main(PSInput psInput) : SV_Target0
{
	float4 sampledColor = (HasBaseColorTex) ? BaseColorTex.Sample(Sampler_BaseColorTex, psInput.uv) : float4(1.0, 1.0, 1.0, 1.0);
	sampledColor.rgb = pow(sampledColor.rgb, float3(2.2, 2.2, 2.2));
	sampledColor *= BaseColorFactor;

	const float3 baseColor = sampledColor.rgb;

	float metallic = (HasMetallicRoughnessTex) ? MetallicRoughnessTex.Sample(Sampler_MetallicRoughnessTex, psInput.uv).b : 1.0;
	metallic *= MetallicFactor;

	float roughness = (HasMetallicRoughnessTex) ? MetallicRoughnessTex.Sample(Sampler_MetallicRoughnessTex, psInput.uv).g : 1.0;
	roughness *= RoughnessFactor;

	const float alpha = roughness * roughness;

	const float3 n = GetNormal(psInput);
	const float3 v = normalize(ViewerPos - psInput.worldPos);
	const float NdotV = dot(n, v);

	const int num_lights = 1;
	//float3 light_pos[num_lights] = { {0.0, 1000.0, 0.0}, { 1000.0, 0.0, 0.0 }, { -1000.0, 0.0, 0.0 } };
	float3 light_pos[num_lights] = { {10.0, 0.0, 0.0} };
	float4 color = float4(0.0, 0.0, 0.0, 1.0);

	[unroll]
	for (int i = 0; i < num_lights; ++i)
	{
		const float3 l = normalize(light_pos[i] - psInput.worldPos);
		const float3 h = normalize(v + l);

		PBRInput pbrInput;
		pbrInput.alpha = alpha;
		pbrInput.NdotL = dot(n, l);
		pbrInput.NdotV = NdotV;
		pbrInput.NdotH = dot(n, h);
		pbrInput.HdotL = dot(h, l);
		pbrInput.HdotV = dot(h, v);

		{
			// f0 is 0.04 for dielectrics
			float F_dieletric = 0.04 + (1.0 - 0.04) * pow(1.0 - abs(pbrInput.HdotV), 5);

			float3 metal_brdf = Specular_brdf(pbrInput) * Fresnel_schlick(baseColor.rgb, pbrInput.HdotV);
			float3 dielectric_brdf = lerp(Diffuse_brdf(baseColor.rgb), Specular_brdf(pbrInput), F_dieletric);

			// The BRDF of the metallic-roughness material is a linear interpolation of a metallic BRDF and a dielectric BRDF.
			float3 material = lerp(dielectric_brdf, metal_brdf, metallic);
			color.rgb += material;
		}

		// We can simplify the mix and arrive at the final BRDF for the material
		//{
		//	float3 c_diff = lerp(baseColor.rgb, float3(0.0, 0.0, 0.0), metallic);
		//	float3 f0 = lerp(float3(0.04, 0.04, 0.04), baseColor.rgb, metallic);
		//	float a = roughness * roughness;

		//	float3 F = f0 + (1 - f0) * pow(1 - abs(pbrInput.HdotV), 0.5);

		//	float3 f_diffuse = (1 - F) / PI * c_diff;
		//	float3 f_specular = F * Specular_brdf(pbrInput);
		//	float3 material = f_diffuse + f_specular;
		//	color.rgb += material;
		//}
	}

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