struct GSOutput
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

float4 main(GSOutput gsOutput) : SV_Target0
{
	return float4(gsOutput.color, 1.0f);
}