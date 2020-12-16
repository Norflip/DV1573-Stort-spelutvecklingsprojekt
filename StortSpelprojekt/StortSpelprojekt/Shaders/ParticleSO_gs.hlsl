SamplerState samLinear : register(s0);
Texture1D randomTex : register(t0);
//Texture2D randomTex : register(t0);

cbuffer cbPerFrame : register(b0)
{
	float4 particleColor;
	float3 eyePosW;
	float3 emitPosW;
	float3 emitDirW;
	float gameTime;
	float timeStep;
	float particleMaxAge;
	row_major matrix viewProj;
	bool usingTexture;
	float3 particleSpreadMulti;
	int particlesPerSecond;
	float2 particleSize;
	bool active;
}

float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture. [-1,1]
	float u = (gameTime + offset);
	float3 v = randomTex.SampleLevel(samLinear, u, 0).xyz;
	//float3 v2 = randomTex.Sample(samLinear, u).xyz;

	return normalize(v);
}

#define PT_EMITTER 0
#define PT_FLARE 1

struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age		   : AGE;
	uint Type          : TYPE;
};

[maxvertexcount(4)]
void main(point Particle gin[1], inout PointStream<Particle> ptStream)
{	
	gin[0].Age += timeStep;

	
	if (gin[0].Type == PT_EMITTER)
	{
		if (active)
		{
			// Emit a new particle
			if (gin[0].Age > 0.005f /*(1.0f / (float)particlesPerSecond)*/ /* 0.005f */)	// particlesPerSecond
			{
				float3 vRandom = RandUnitVec3(0.0f);
				vRandom *= particleSpreadMulti;

				Particle p;
				p.InitialPosW = emitPosW.xyz + vRandom;		// gEmitPosW.xyz;
				p.InitialVelW = emitDirW + vRandom;			// 3.0f * vRandom;		float3(0, 1, 0) + vRandom
				p.SizeW = particleSize;							// float2(3.0f, 3.0f);
				p.Age = gin[0].Age;
				p.Type = PT_FLARE;

				ptStream.Append(p);

				// reset the time to emit
				gin[0].Age = 0.0f;
			}
		}
			// always keep emitters
			ptStream.Append(gin[0]);
		
	}
	else
	{
		if (active)
		{
		// Specify conditions to keep particle
		if (gin[0].Age <= particleMaxAge)		// max age
			ptStream.Append(gin[0]);
		}
	}
	
}

GeometryShader gsStreamOut = ConstructGSWithSO(
	CompileShader(gs_5_0, main()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");