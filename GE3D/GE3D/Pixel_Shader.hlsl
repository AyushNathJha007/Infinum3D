cbuffer ConstBuf
{
	float4 faceColors[6];
};

float4 main(uint TriangleID:SV_PrimitiveID) : SV_Target //This will tell the pipeline to generate a unique ID for every triangle, which will be passed into pixel shader
{
	return faceColors[TriangleID/2]; //Two triangles make one face of the cube
}