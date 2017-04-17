cbuffer MatrixBuffer
{
	matrix xWorldMatrix;
	matrix xViewMatrix;
	matrix xProjectionMatrix;
	matrix xLightViewMatrix;
	matrix xLightProjectionMatrix;
};

cbuffer LightBuffer2
{
	float3 f3LightDirection;
	float fPadding;
};

struct TerrainVertexInput
{
	float4 f4Position : POSITION;
	float4 f4Color : COLOR;
	float3 f3Normal : NORMAL;
};

struct TerrainPixelInput
{
	float4 f4Position : SV_POSITION;
	float4 f4Color : COLOR;
	float3 f3Normal : NORMAL;
    float4 f4LightViewPosition : TEXCOORD1;
	float3 f3LightPos : TEXCOORD2;
};

TerrainPixelInput TerrainVertexShader( TerrainVertexInput xInput )
{
	TerrainPixelInput xOutput;
	float4 f4WorldPosition;

	xInput.f4Position.w = 1.0f;

	xOutput.f4Position = mul(xInput.f4Position, xWorldMatrix);
	xOutput.f4Position = mul(xOutput.f4Position, xViewMatrix);
	xOutput.f4Position = mul(xOutput.f4Position, xProjectionMatrix);

	xOutput.f4LightViewPosition = mul(xInput.f4Position, xWorldMatrix);
    xOutput.f4LightViewPosition = mul(xOutput.f4LightViewPosition, xLightViewMatrix);
    xOutput.f4LightViewPosition = mul(xOutput.f4LightViewPosition, xLightProjectionMatrix);

	xOutput.f4Color = xInput.f4Color;

	xOutput.f3Normal = mul(xInput.f3Normal, (float3x3)xWorldMatrix);

	xOutput.f3Normal = normalize(xOutput.f3Normal);

	f4WorldPosition = mul(xInput.f4Position, xWorldMatrix);

	xOutput.f3LightPos = -f3LightDirection;

	xOutput.f3LightPos = normalize(xOutput.f3LightPos);

	return xOutput;
}