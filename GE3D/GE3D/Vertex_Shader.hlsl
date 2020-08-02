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

/*Note:- HLSL files expect our Matrix to be column major*/
//Defines ConstantBuffer for use by this vertex shader
cbuffer CBuf
{
	matrix transformMat;	//matrix indicates 4x4 float
	//Now, we have already taken the transpose of the matrix while doing operations in
	//the .cpp file. So, we now have a column major matrix.
}

VSOutput main(float2 pos:Position,float3 color:Color)
{
	VSOutput v;
	v.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f),transformMat);
	v.color = color;
	return v;
}