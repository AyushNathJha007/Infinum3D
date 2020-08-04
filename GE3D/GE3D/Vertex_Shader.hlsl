//A shader is kinda a pure function

//Please note that color here in Vertex Shader is binded to register zero->
//Our vertex shader doesn't take color now.

/*Note:- HLSL files expect our Matrix to be column major*/
//Defines ConstantBuffer for use by this vertex shader
cbuffer CBuf
{
	matrix transformMat;	//matrix indicates 4x4 float
	//Now, we have already taken the transpose of the matrix while doing operations in
	//the .cpp file. So, we now have a column major matrix.
}

float4 main(float3 pos:Position):SV_Position
{
	
	return mul(float4(pos.x, pos.y,pos.z, 1.0f),transformMat);
	
}