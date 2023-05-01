cbuffer NormalGSConstants : register(b0)
{
	float Scale;
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

struct VSOutput
{
	float4 position : SV_POSITION;
	float3 worldPos : POSITION;
	float3 normal : NORMAL;
	//float3 tangent : TANGENT;
	float2 uv : TEXCOORD0;
};

struct GSOutput
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

[maxvertexcount(2)]
void main(point VSOutput vsInput[1], inout LineStream<GSOutput> gsOutput)
{
	GSOutput element;
	element.position = vsInput[0].position;
	element.color = float3(1.0f, 1.0f, 0.0f);
	gsOutput.Append(element);

	element = (GSOutput)0;
	element.position = float4(vsInput[0].worldPos + vsInput[0].normal * Scale, 1.0f);
	element.position = mul(ViewProjMatrix, element.position);
	element.color = float3(1.0f, 0.0f, 0.0f);
	gsOutput.Append(element);
}