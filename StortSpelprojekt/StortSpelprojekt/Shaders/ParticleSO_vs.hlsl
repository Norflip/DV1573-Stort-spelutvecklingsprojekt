struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age : AGE;
	uint Type          : TYPE;
};

struct ParticleOut
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age		   : AGE;
	uint Type          : TYPE;
};

ParticleOut StreamOutVS(Particle vin)
{
	ParticleOut pOut;
	pOut.InitialPosW = vin.InitialPosW;
	pOut.InitialVelW = vin.InitialVelW;
	pOut.SizeW = vin.SizeW;
	pOut.Age = vin.Age;
	pOut.Type = vin.Type;

	return pOut;
}