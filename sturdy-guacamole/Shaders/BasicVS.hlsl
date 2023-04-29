static const float PI = 3.141592;
static const float Epsilon = 0.00001;

cbuffer MeshConstants : register(b0)
{
	float4x4 WorldMatrix;		// Model to world
	float4x4 WorldIT;			// Model normals to world
}
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

struct VSInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD0;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION;
};

VSOutput main(VSInput vsInput) 
{   
	VSOutput vsOutput;

	float4 position = float4(vsInput.position, 1.0);
	vsOutput.worldPos = mul(WorldMatrix, position).xyz;
	vsOutput.position = mul(ViewProjMatrix, float4(vsOutput.worldPos, 1.0));
	return vsOutput;
}