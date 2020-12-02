
cbuffer Emitter : register (b0)
{
	float3 emitterPosition;
	float time;
	float lifeTime;
	float3 pad_emitter_0;
}

struct ParticleInput
{
	float3 position;
	float speed;
	float timeOffset;
	float3 pi_pad_0;
};

struct ParticleOutput
{
	float3 position;
	float po_pad_0;
};

StructuredBuffer<ParticleInput> inputData : register(t0);
RWStructuredBuffer<ParticleOutput> outputData : register(u0);


[numthreads(10, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	//uint myID = DispatchThreadID.x + DispatchThreadID.y * 10 + DispatchThreadID.z * 10 * 10;
	ParticleInput input = inputData[DTid.x];


	//float3 pos = startPos + startVelocity * t + (acceleration * t * t) * 0.5f;
	float3 pos = input.position + emitterPosition;
	pos.y = pos.y + ((input.timeOffset + time) % lifeTime) * input.speed; // speed


	ParticleOutput output;
	output.position = pos;
	output.po_pad_0 = 0;
	outputData[DTid.x] = output;
}