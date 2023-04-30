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

struct VSOutput
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	float2 texCoord : TEXCOORD0;
};

float4 main(VSOutput vsOutput) : SV_Target0
{
	return float4(0.75f, 0.75f, 0.75f, 1.0f);
}