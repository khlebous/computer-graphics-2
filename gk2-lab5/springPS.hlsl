#define NLIGHTS 2

texture2D normTex : register(t0);
sampler samp : register(s0);

float4 lightPos[NLIGHTS];
float3 lightColor[NLIGHTS];
float3 surfaceColor;
float ks, kd, ka, m;

float3 normalMapping(float3 N, float3 T, float3 tn)
{
	float3 B = cross(N, T) / length(cross(N, T));
	T = cross(B, N);
	float3x3 m = transpose(float3x3(T, B, N));

	return mul(m, tn);
}

float4 phong(float3 worldPos, float3 norm, float3 view)
{
	view = normalize(view);
	norm = normalize(norm);
	float3 color = surfaceColor * ka; //ambient
	for (int k = 0; k < NLIGHTS; ++k)
	{
		float3 lightVec = normalize(lightPos[k].xyz - worldPos);
		float3 halfVec = normalize(view + lightVec);
		color += lightColor[k] * kd * surfaceColor * saturate(dot(norm, lightVec));//diffuse
		color += lightColor[k] * ks * pow(saturate(dot(norm, halfVec)), m);//specular
	}
	return saturate(float4(color, 1.0f));
}

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 worldPos : POSITION0;
	float3 norm : NORMAL0;
	float3 view : VIEWVEC0;
	float2 tex : TEXCOORD0;
	float3 tangent : TANGENT0;
};

float4 main(PSInput i) : SV_TARGET
{
	float3 N = normalize(i.norm);
	float3 T = normalize(i.tangent);
	float3 tn = normTex.Sample(samp, i.tex);
	tn = tn * 2 - 1;
	float3 norm = normalMapping(N, T, tn);

	return phong(i.worldPos, norm, i.view);
}