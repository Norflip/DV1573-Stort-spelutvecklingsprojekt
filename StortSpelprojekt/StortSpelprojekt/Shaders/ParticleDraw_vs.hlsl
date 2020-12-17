cbuffer cbFixed
{
	float3 accelerationW = { 0.0f, 7.8f, 0.0f };
};

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
};

struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age		   : AGE;
	uint Type          : TYPE;
};

struct VertexOut
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint   Type  : TYPE;
	float Age	 : AGE;
};

VertexOut main(Particle vin)
{
	VertexOut vout;

	float t = vin.Age;


	float yAmplitude = .8f;
	float zAmplitude = .8f;
	float offset = 5;
	float x = zAmplitude * sin(vin.Age / 1 * offset);
	float y = zAmplitude * sin(vin.Age / 0.5f * offset);

	vout.PosW = 0.5f * t * t * accelerationW + t * vin.InitialVelW + vin.InitialPosW; // +float3(0, y, 0);

	// fade color with time 
	float a = t / 1.0f;
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, a);
	//vout.Color = float4(0.5f, 0.2f, 0.2f, opacity);
	vout.Color = particleColor;
	vout.Color.a = opacity;
	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;
	vout.Age = vin.Age;

	return vout;
}