//Pixel Shader sets the color of the pixel on the raster
//The rasterizer determines the pixel positions; The pixel shader determines the colors of each of those pixels.
//It usually doesn't need the coordinate of a pixel in screen space to accomplish this task.
//Hence, the function for pixel shader has no input values like position of pixel.

float4 main(float3 color:Color) : SV_Target //SystemValue_Target-> this means Render Target
{
	return float4(color,1.0f); //Output the color-> RGBA
}