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

VSOutput main(float2 pos:Position,float3 color:Color)
{
	VSOutput v;
	v.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	v.color = color;
	return v;
}