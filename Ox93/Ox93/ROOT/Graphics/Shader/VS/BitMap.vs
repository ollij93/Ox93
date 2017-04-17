//////////////////////////////////////////////////////
// B I T M A P   V E R T E X   S H A D E R
//////////////////////////////////////////////////////

// Structures...
cbuffer MatrixBuffer
{
	matrix xWorldMatrix;
	matrix xViewMatrix;
	matrix xOrthoMatrix;
};

struct BitMapVertexInput
{
	float4 f4Position : POSITION;
	float2 f2Tex : TEXCOORD0;
	float4 f4Normal : NORMAL;
};

struct BitMapPixelInput
{
	float4 f4Position : SV_POSITION;
	float2 f2Tex : TEXCOORD0;
	float4 f4Normal : NORMAL;
};

// Functions...
BitMapPixelInput BitMapVertexShader(BitMapVertexInput xInput)
{
	BitMapPixelInput xOutput;

	xInput.f4Position.w = 1.0f;

	xOutput.f4Position = mul(xInput.f4Position, xWorldMatrix);
	xOutput.f4Position = mul(xOutput.f4Position, xViewMatrix);
	xOutput.f4Position = mul(xOutput.f4Position, xOrthoMatrix);

	xOutput.f2Tex = xInput.f2Tex;

	return xOutput;
}