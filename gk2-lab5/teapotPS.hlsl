#define NLIGHTS 2

texture2D normTex : register(t0);
sampler samp : register(s0);

static const float PI = 3.14159265359;

float4 lightPos[NLIGHTS];
float3 lightColor[NLIGHTS];
float4 camPos;

float3 albedo;
float metallic;
float roughness;

float normalDistributionGGX(float3 N, float3 H)
{
	float r2 = roughness * roughness;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (r2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return r2 / denom;
}

float3 normalMapping(float3 N, float3 T, float3 tn)
{
	float3 B = cross(N, T) / length(cross(N, T));
	T = cross(B, N);
	float3x3 m = float3x3(T, B, N);

	return mul(tn, m);
}
struct PSInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION0;
	float3 norm : NORMAL0;
	float3 view : VIEWVEC0;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput i) : SV_TARGET
{
	float3 A = pow(albedo, 2.2);

	float3 F0 = float3(0.04f, 0.04f, 0.04f) * (1 - metallic) + A * metallic;

	float3 NN = normalize(i.norm);
	float3 dPdx = ddx(i.worldPos);
	float3 dPdy = ddy(i.worldPos);
	float2 dtdx = ddx(i.tex);
	float2 dtdy = ddy(i.tex);
	float3 T = normalize(-dPdx * dtdy.y + dPdy * dtdx.y);
	float3 tn = normTex.Sample(samp, i.tex);
	tn = tn * 2 - 1;
	tn.y = -tn.y;

	float3 norm = normalMapping(NN, T, tn);

	float3 N = normalize(norm);
	float3 V = normalize(i.view);

	float3 Li = float3(0.0, 0.0, 0.0);
	for (int idx = 0; idx < NLIGHTS; idx++)
	{
		int idx = 0;

		float3 L = normalize(lightPos[idx] - i.worldPos);
		float3 H = normalize(V + L);

		float distance = length(lightPos[idx] - i.worldPos);
		float attenuation = 1.0 / (distance * distance);
		float Il = lightColor[idx] * attenuation;

		float3 radiance = Il * max(dot(N, L), 0.0);

		float NDF = normalDistributionGGX(N, H);
		return float4(NDF, NDF, NDF, 1.0f);
	}
}