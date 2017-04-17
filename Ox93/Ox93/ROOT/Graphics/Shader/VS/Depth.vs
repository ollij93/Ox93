cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

PixelInputType DepthVertexShader(VertexInputType xInput)
{
	PixelInputType xOutput;

	xInput.position.w = 1.f;
	
	xOutput.position = mul(xInput.position, worldMatrix);
    xOutput.position = mul(xOutput.position, viewMatrix);
    xOutput.position = mul(xOutput.position, projectionMatrix);

	xOutput.depthPosition = xOutput.position;

	return xOutput;
}
