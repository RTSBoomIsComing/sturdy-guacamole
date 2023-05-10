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
	float3 position	: POSITION;
	float3 normal	: NORMAL;
	float3 tangent	: TANGENT;
	float2 uv	: TEXCOORD0;
};

struct VSOutput
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 uv : TEXCOORD0;
};

VSOutput main(VSInput vsInput) 
{   
	VSOutput vsOutput;

	// ** We are using column major matrices here **
	
	// Transform the vertex position from model space to world space
	vsOutput.worldPos = mul(WorldMatrix, float4(vsInput.position, 1.0)).xyz;

	// Transform the vertex position from world space to homogeneous clip space
	vsOutput.position = mul(ViewProjMatrix, float4(vsOutput.worldPos, 1.0));
	
	// Transform the vertex normal from model space to world space
	{
		vsOutput.normal = mul(WorldIT, float4(vsInput.normal, 0.0)).xyz;
		vsOutput.normal = normalize(vsOutput.normal);
	}

	// Transform the vertex tangent from model space to world space
	{
		vsOutput.tangent = mul(WorldMatrix, float4(vsInput.tangent, 0.0)).xyz;
		vsOutput.tangent = normalize(vsOutput.tangent);
	}

	// Pass the vertex texture coordinates to next stage
	vsOutput.uv = vsInput.uv;

	return vsOutput;
}