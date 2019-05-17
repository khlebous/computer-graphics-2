float4 camPos;
sampler samp;

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 localPos : POSITION0;
	float3 worldPos : POSITION1;
};

float4 main(PSInput i) : SV_TARGET
{
	float3 viewVec = normalize(camPos.xyz - i.worldPos);
	float3 norm = float3(0.0f, 1.0f, 0.0f);

	float3 color = reflect(-viewVec, norm);
	//float3 color = refract(-viewVec, norm, 0.75f);

	return float4(color, 1.0f);
}