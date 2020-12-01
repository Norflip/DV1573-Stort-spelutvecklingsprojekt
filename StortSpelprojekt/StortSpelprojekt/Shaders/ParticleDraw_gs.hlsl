//Texture2D textureshit : register(t0);
//SamplerState samLinear : register(s0);

//cbuffer cbPerObject : register(b0)
//{
//	row_major matrix worldViewProjection;
//	row_major matrix worldspace;
//	row_major matrix viewspace;
//	row_major matrix projectionspace;
//	row_major float4x4 viewProjection;
//	
//};

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
};

//cbuffer cbFixed
//{
//	float3 accelerationW = { 0.0f, 7.8f, 0.0f };
//};

struct VertexOut
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
	float4 Color : COLOR;
	uint   Type  : TYPE;
	float Age : AGE;
};

struct GeoOut
{
	float4 PosH  : SV_Position;
	float4 Color : COLOR;
	float2 Tex   : TEXCOORD;
	float Age : AGE;
};

#define PT_EMITTER 0
#define PT_FLARE 1

// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void main(point VertexOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
	// do not draw "emitter" particles.
	if (gin[0].Type != PT_EMITTER) {

		// Compute world matrix so that billboard faces the camera		
		float3 look = normalize(eyePosW.xyz - gin[0].PosW);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);

		// Compute triangle strip vertices (quad) in world space
		float halfWidth = 0.5f * gin[0].SizeW.x;
		float halfHeight = 0.5f * gin[0].SizeW.y;

		float4 v[4];
		v[0] = float4(gin[0].PosW + halfWidth * right - halfHeight * up, 1.0f);
		v[1] = float4(gin[0].PosW + halfWidth * right + halfHeight * up, 1.0f);
		v[2] = float4(gin[0].PosW - halfWidth * right - halfHeight * up, 1.0f);
		v[3] = float4(gin[0].PosW - halfWidth * right + halfHeight * up, 1.0f);


		GeoOut gOut[4];
		gOut[0].PosH = mul(viewProj, v[0]);
		gOut[1].PosH = mul(viewProj, v[1]);
		gOut[2].PosH = mul(viewProj, v[2]);
		gOut[3].PosH = mul(viewProj, v[3]);

		gOut[0].Tex = float2(0.0f, 1.0f);
		gOut[1].Tex = float2(1.0f, 1.0f);
		gOut[2].Tex = float2(0.0f, 0.0f);
		gOut[3].Tex = float2(1.0f, 0.0f);

		gOut[0].Color = gin[0].Color;
		gOut[1].Color = gin[0].Color;
		gOut[2].Color = gin[0].Color;
		gOut[3].Color = gin[0].Color;

		gOut[0].Age = gin[0].Age;
		gOut[1].Age = gin[0].Age;
		gOut[2].Age = gin[0].Age;
		gOut[3].Age = gin[0].Age;

		for (int i = 0; i < 4; ++i)
		{
			triStream.Append(gOut[i]);
		}

		// Transform quad vertices to world space and output 
		// them as a triangle strip.		

		/*GeoOut gout;
		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			gout.PosH = mul(viewProj, v[i]);
			gout.Tex = gQuadTexC[i];
			gout.Color = gin[0].Color;
			triStream.Append(gout);
		}*/
	}
}