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


// The separable form of the Smith joint masking - shadowing function
float G_Smith(float NdotL, float NdotV, float alpha)
{
	float alpha2 = alpha * alpha;
	float shadowing = 2.0 * NdotL / (NdotL + sqrt(alpha2 + (1.0 - alpha2) * (NdotL * NdotL)));
	float masking = 2.0 * NdotV / (NdotV + sqrt(alpha2 + (1.0 - alpha2) * (NdotV * NdotV)));
	return shadowing * masking;
}

// The Trowbridge-Reitz/GGX microfacet distribution
float D_GGX(float NdotH, float alpha)
{
	float f = (NdotH * NdotH * (alpha * alpha - 1.0) + 1.0);
	return alpha * alpha / (PI * f * f);
}

// The specular reflection is a microfacet BRDF
// microfacet BRDF = G * D / (4 * NdotL * NdotV)
float3 Specular_brdf(float alpha, float NdotL, float NdotV, float NdotH)
{
	return G_Smith(NdotL, NdotV, alpha) * D_GGX(NdotH, alpha) / (4.0 * NdotL * NdotV);
}

// The diffuse reflection is a Lambertian BRDF
float3 Diffuse_brdf(float3 color)
{
	return (1 / PI) * color;
}

float4 main(PSInput psInput) : SV_Target0
{
	float4 sampledColor = (HasBaseColorTex) ? BaseColorTex.Sample(Sampler_BaseColorTex, psInput.uv) : float4(1.0, 1.0, 1.0, 1.0);
	sampledColor.rgb = pow(sampledColor.rgb, float3(2.2, 2.2, 2.2));
	sampledColor *= BaseColorFactor;

	float3 baseColor = sampledColor.rgb;

	float metallic = (HasMetallicRoughnessTex) ? MetallicRoughnessTex.Sample(Sampler_MetallicRoughnessTex, psInput.uv).b : 1.0;
	metallic *= MetallicFactor;

	float roughness = (HasMetallicRoughnessTex) ? MetallicRoughnessTex.Sample(Sampler_MetallicRoughnessTex, psInput.uv).g : 1.0;
	roughness *= RoughnessFactor;

	float alpha = roughness * roughness;

	float3 n = GetNormal(psInput);
	float3 v = normalize(ViewerPos - psInput.worldPos);
	float NdotV = clamp(dot(n, v), 0.001, 1.0);

	const int num_lights = 3;
	float3 light_pos[num_lights] = { {0.0, 1000.0, 0.0}, { 1000.0, 0.0, 0.0 }, { -1000.0, 0.0, 0.0 } };
	float4 color = float4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < num_lights; ++i)
	{
		float3 l = normalize(light_pos[i] - psInput.worldPos);
		float3 h = normalize(v + l);

		float NdotL = clamp(dot(n, l), 0.001, 1.0);
		float NdotH = clamp(dot(n, h), 0.0, 1.0);
		float VdotH = clamp(dot(v, h), 0.0, 1.0);
		//float LdotH = clamp(dot(l, h), 0.0, 1.0);

		float3 metal_brdf = Specular_brdf(alpha, NdotL, NdotV, NdotH) 
			* (baseColor.rgb + (float3(1.0, 1.0, 1.0) - baseColor.rgb) * pow(1.0 - VdotH, 5));

		float3 dielectric_brdf = lerp(Diffuse_brdf(baseColor.rgb), Specular_brdf(alpha, NdotL, NdotV, NdotH),
			0.04 + (1.0 - 0.04) * pow(1.0 - VdotH, 5));

		// The BRDF of the metallic-roughness material is a linear interpolation of a metallic BRDF and a dielectric BRDF.
		float3 material = lerp(dielectric_brdf, metal_brdf, metallic);
		color.rgb += material;

		// We can simplify the mix and arrive at the final BRDF for the material
		//float3 c_diff = lerp(baseColor.rgb, float3(0.0, 0.0, 0.0), metallic);
		//float3 f0 = lerp(float3(0.04, 0.04, 0.04), baseColor.rgb, metallic);
		//float a = roughness * roughness;

		//float3 F = f0 + (1 - f0) * pow(1 - VdotH, 0.5);

		//float3 f_diffuse = (1 - F) / PI * c_diff;
		//float3 f_specular = F * Specular_brdf(alpha, NdotL, NdotV, NdotH);
		//float3 material = f_diffuse + f_specular;
		//color.rgb += material;
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