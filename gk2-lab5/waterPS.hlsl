textureCUBE envMap;
float4 camPos;
sampler samp;

const float f0 = 0.17f;

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 localPos : POSITION0;
	float3 worldPos : POSITION1;
};

float3 IntersectRay(float3 pos, float3 dir)
{
	float3 v_a = max((1 - pos) / dir, (-1 - pos) / dir);
	float a = min(v_a.x, min(v_a.y, v_a.z));

	return pos + a * dir;
}

float Fresnel(float cos)
{
	return f0 + (1 - f0) * pow(1 - cos, 5);
}

float4 main(PSInput i) : SV_TARGET
{
	float3 viewVec = normalize(camPos.xyz - i.worldPos);
	float3 norm = float3(0.0f, 1.0f, 0.0f);
	float n = 0.75;

	if (dot(viewVec, norm) < 0) 
	{
		norm = -norm;
		n = 1 / n;
	}

	float3 v_reflect = IntersectRay(i.localPos, reflect(-viewVec, norm));
	float3 reflectColor = envMap.Sample(samp, v_reflect);

	float3 v_refract = IntersectRay(i.localPos, refract(-viewVec, norm, 0.75f));
	float3 refractColor = envMap.Sample(samp, v_refract);

	float fresnel_coeff = Fresnel(dot(viewVec, norm));
	float3 color = fresnel_coeff * reflectColor + (1 - fresnel_coeff) * refractColor;

	return float4(pow(color, 0.4545f), 1.0f);
}
