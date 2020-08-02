//A shader is kinda a pure function

//Please note that color here in Vertex Shader is binded to register zero->
//The color should be binded to reg zero also in Pixel Shader.
//Otherwise, it will throw error. So that's why in this struct, we first add color and then position
//to match compatibility with Pixel Shader.

struct VSOutput
{
	float3 color:Color;
	float4 pos:SV_Position;
	
};

//Defines ConstantBuffer for use by this vertex shader
cbuffer CBuf
{
	row_major matrix transformMat;	//matrix indicates 4x4 float
	//We add row_major keyword to tell directx that this is a row major matrix. Hence, while performing
	//multiplication as below, the calculations done are consistent.

	//This inconsistency arises of the fact that in CPU. 2D arrays are stored in row_major.
	//While is GPU, these are treated as column major.
	//So, we need to explicitly specify that our matrix here is row major.
}

VSOutput main(float2 pos:Position,float3 color:Color)
{
	VSOutput v;
	v.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f),transformMat);
	v.color = color;
	return v;
}