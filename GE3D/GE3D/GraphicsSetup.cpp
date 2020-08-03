#include "GraphicsSetup.h"
#include "dxerr.h"
#include<sstream>
#include<d3dcompiler.h>
#include<cmath>
#include<DirectXMath.h>


#pragma comment(lib,"D3DCompiler.lib")	//Can be used to compile shaders at runtime; but we will be using for shader loading function
#pragma comment(lib,"d3d11.lib")

//Additional Macros for handling GraphicsSetup exception throw stuff (Now also includes DXGI Info)
#define GFX_EXCEPT_NOINFO(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( HR = (hrcall) ) ) throw GraphicsSetup::HRException( __LINE__,__FILE__,HR )

#ifndef NDEBUG	//Additional info messages will be shown, when in debug mode. Else, additional info won't be shown.
#define GFX_EXCEPT(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( HR = (hrcall) ) ) throw GFX_EXCEPT(HR)
#define GFX_DEVICE_REMOVED_EXCEPT(HR) GraphicsSetup::DeviceRemovedException( __LINE__,__FILE__,(HR),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v=infoManager.GetMessages(); if(!v.empty()){throw GraphicsSetup::InfoOnlyException(__LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(HR) GraphicsSetup::DeviceRemovedException(__LINE__,__FILE__,(HR))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

namespace wrl = Microsoft::WRL;
namespace dxMath = DirectX;
/*Function to create DEVICE object*/
GraphicsSetup::GraphicsSetup(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	//Configuring Buffer->
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0; //Setting both height and width to 0 means it will figure it out itself based on window size.
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;	//This format is the layout of pixels, with channels in there(it's a BGRA layout)
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;	//Pick whatever refresh rate is already there
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//Configuring Sampling mode(Anti-aliasing) stuff->
	sd.SampleDesc.Count = 1;	//We don't need Anti-Aliasing as of now
	sd.SampleDesc.Quality = 0;
	//More information on Buffer->
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//We want this buffer to be used as render target
	sd.BufferCount = 1;	//We want one front buffer and one back buffer i.e double buffering. So we set this to 1.(Confusing eh?)
	sd.OutputWindow = hWnd;
	sd.Windowed = true;
	//Effect used for flipping presentation->
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //Gives best performance in most states
	sd.Flags = 0;
	UINT swapCreateFlags = 0u;

	//Create a device on DebugLayer only if we are in debug mode
	
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT HR; //Added it here, because our additional macros defined at document's start expects it as a local variable

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,	//Which adapter do we want to choose -> Select the default one
		D3D_DRIVER_TYPE_HARDWARE,	//Choose Hardware Device
		nullptr,	//Handle to the binary for software driver->nullptr means default
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,	//Target SDK Version
		&sd,	//Pointer to a descriptor structure
		&pSwap,
		&pDevice,
		nullptr,
		&pContext
	));
	/*A swap chain is basically a collection of subresources like textures etc
	The below pointer is used to Gain access to texture subresource in swap chain(back buffer)*/
	wrl::ComPtr<ID3D11Resource> pBackBuffer;
	
	GFX_THROW_INFO(pSwap->GetBuffer(
		0, //Index of the buffer we wanna get
		__uuidof(ID3D11Resource), //uuid of the interface on which we wanna receive the sub-object
		(&pBackBuffer) //Used to fill the pointer (Refer COM)
	));
	//Once we have a handle to the buffer as a D3D11Resource, we can use that to create a RenderTargetView on that resource
	GFX_THROW_INFO(pDevice->CreateRenderTargetView(
		pBackBuffer.Get(), //We create a rendertargetview on the back buffer
		nullptr,
		&pTarget
	));
	
}

/*Destructor deleted-> No longer needed*/

void GraphicsSetup::EndFrame()
{
	HRESULT HR;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(HR = pSwap->Present(1u, 0u)))
	{
		if (HR == DXGI_ERROR_DEVICE_REMOVED) //Special error code; Has Additional Information which can be extracted using GetDeviceRemovedReason()
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(HR);
		}
	}
	//pSwap->Present(1u, 0u); //Setting frame rate interval to 1, and 0 flags
}

void GraphicsSetup::ClearBuffer(float red, float green, float blue) noexcept
{
	
		const float color[] = { red,green,blue,1.0f };
		pContext->ClearRenderTargetView(pTarget.Get(), color);
	
}

/*This function is going to be updated for checking vertex transformation via rotation matrix with the help of vertex shader*/
void GraphicsSetup::DrawTriangleTest(float angle, float x, float y)
{
	namespace wrl = Microsoft::WRL;
	HRESULT HR;
	//Index Drawing->Specify the set of vertices once, then use a set of indexes to select them in some order to draw stuff
	struct Vertex {	//Structure of a vertex we'll be using(x,y and z coordinates)
		struct {
			float x;
			float y;
			float z;
		}pos;
		struct {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		}color;
	};

	//Create vertex buffer (a 2D triangle)
	//We have taken 6 vertices here, to experiment with a hexagon using indexed drawing
	const Vertex VerticesData[] = { {-1.0,-1.0,-1.0, 0, 255, 0},
		{1.0,-1.0,-1.0, 255, 0, 0},
		{-1.0,1.0,-1.0,0,0,255},
		{1.0,1.0,-1.0, 0, 255, 0},
		{-1.0,-1.0,1.0, 255,0,0},
		{1.0,-1.0,1.0,0,255,0},
		{-1.0,1.0,1.0, 0,0,255},
		{1.0,1.0,1.0, 255,0,0},//These vertices, when taken in this order, are clockwise winded. So, the pipeline won't do back face culling for this.
	//{0.5,0.5},{-0.5,0.5} ,{0.0,-0.5}	//These vertices are binded in anticlockwise direction. Hence, BackFaceCulling is done by pipeline by default.
	};
	wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
	D3D11_BUFFER_DESC bd = {};

	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(VerticesData);
	bd.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA sd = {};

	sd.pSysMem = VerticesData;

	GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

	

	//Bind VertexBuffer to pipeline
	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	//IASetVertexBuffers() sets the VertexBuffer.
	//Using this single function, we can set multiple vertex buffers.
	pContext->IASetVertexBuffers(0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset); //The 2 initial letters correspond to the stage in the pipeline. IA stands for Input Assembler.

	//Create Index Buffer
	const unsigned short indices[] =	//Indices are 16 bit by default. So we take unsigned short.
	{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
	};
	wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
	D3D11_BUFFER_DESC IndexBufferDesc = {};
	IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	IndexBufferDesc.CPUAccessFlags = 0u;
	IndexBufferDesc.MiscFlags = 0u;
	IndexBufferDesc.ByteWidth = sizeof(indices);
	IndexBufferDesc.StructureByteStride = sizeof(unsigned short);

	D3D11_SUBRESOURCE_DATA IndexSubResourceDat = {};

	IndexSubResourceDat.pSysMem = indices;

	GFX_THROW_INFO(pDevice->CreateBuffer(&IndexBufferDesc, &IndexSubResourceDat, &pIndexBuffer));

	//Bind IndexBuffer to pipeline
	pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	/*We create a constant buffer for Matrix Transformation*/
	struct ConstantBuffer
	{
		dxMath::XMMATRIX TransformationMatrix;	//A 4x4 Matrix. But its elements can't be accessed directly, since it is optimized for SIMD
	};

	const ConstantBuffer constBuffer =	//Rotation Matrix (Z is Rotation Axis)
	{
		{
			dxMath::XMMatrixTranspose(
			
			dxMath::XMMatrixRotationZ(angle)*
				dxMath::XMMatrixScaling(3.0f / 4.0f,1.0f,1.0f)*dxMath::XMMatrixRotationX(angle)*dxMath::XMMatrixTranslation(x,y,4.0f)*dxMath::XMMatrixPerspectiveLH(1.0f,1.0f,0.5f,10.0f))
		}
		//Matrix from DirectXMath are row major
		//(3/4) has been multiplied to squeeze our vertices to fit our aspect ratio of 3:4 (600x800).
	};

	wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
	D3D11_BUFFER_DESC ConstantBufferDesc = {};
	ConstantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ConstantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;	//Using Dynamic, Since constant buffer upates every frame
	ConstantBufferDesc.CPUAccessFlags =D3D11_CPU_ACCESS_WRITE;	//Since our dynamic resource will be updated every frame by the CPU, we need to give Write permission
	ConstantBufferDesc.MiscFlags = 0u;
	ConstantBufferDesc.ByteWidth = sizeof(constBuffer);
	ConstantBufferDesc.StructureByteStride = 0u;	//Since this isn't an array, like array of vertices, etc

	D3D11_SUBRESOURCE_DATA ConstantSubResourceDat = {};

	ConstantSubResourceDat.pSysMem = &constBuffer;

	GFX_THROW_INFO(pDevice->CreateBuffer(&ConstantBufferDesc, &ConstantSubResourceDat, &pConstantBuffer));

	//Now, we bind Constant Buffer to vertex buffer
	pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	/*We create Pixel Buffer first then Vertex Buffer. Because we don't want pBlob data to be overwritten by
	Pixel Shader Byte Code because the pBlob data consisting of Vertex Shader Byte Code is later on
	used while creation of Input Layout.*/

	//Create Pixel Buffer
	wrl::ComPtr<ID3D11PixelShader> pPixelShader;
	wrl::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"Pixel_Shader.cso", &pBlob));	//.cso -> Compiled Shader Object
	GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));

	//Bind the Pixel Shader to the pipeline
	pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	//Create Vertex Buffer
	wrl::ComPtr<ID3D11VertexShader> pVertexShader;
	
	GFX_THROW_INFO(D3DReadFileToBlob(L"Vertex_Shader.cso", &pBlob));	//.cso -> Compiled Shader Object
	GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));

	//Bind the Vertex Shader to the pipeline
	pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	

	//Create an input layout object for vertex, and bind it to our pipeline
	wrl::ComPtr<ID3D11InputLayout> pInputLayout;
	//A descriptor is needed to describe the pInputLayout.
	//The descriptor is actually an array of descriptors, as shown below. So, every element
	//in the "Vertex" will have one member as a descriptor in the array of descriptors.
	//In our case, the "Vertex" has two elements x and y of type float. But in terms of 
	//pixel shader, they can be lumped as a single entity, depicting position. So, only one member.
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{"Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0}, //Tells the position of the vertex
		//The first two parameters represent Semantic name and Semantic Index.
		//For this, refer to the Vertex Shader hlsl file. We have only one semantic named
		//"Position" and it is at index 0.
		//The third parameter tells us the type of data is in the element. R32G32B32_FLOAT tells
		//that we have three 32 bit floats (x,y and z).
		{"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}, //Tells the color of the vertex
		//UNORM normalizes the value to full range of input type
		//i.e, an input of 255, when converted to float will represent 1.0 and vice versa
	};

	GFX_THROW_INFO(pDevice->CreateInputLayout(ied, (UINT)std::size(ied), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));

	//Bind Vertex Layout to our pipeline
	pContext->IASetInputLayout(pInputLayout.Get());

	//The below function binds the render target
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), nullptr);

	//The below function sets the primitive topology to triangle vertices
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//We need to configure the viewport for rasterizing
	D3D11_VIEWPORT vport;
	vport.Width = 800;
	vport.Height = 600;
	vport.MinDepth = 0;
	vport.MaxDepth = 1;
	vport.TopLeftX = 0;
	vport.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vport);
	
	//We use DrawIndexed() to do indexed drawing
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices),0u, 0u)); //Takes in parameters-> Number of vertices to draw,and the start vertex (vertices numbered as 0,1,2,..)
}


// GraphicsSetup exception stuff
GraphicsSetup::HRException::HRException(int line, const char * file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file),
	hr(hr)
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* GraphicsSetup::HRException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "Error Code: 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "Error String: " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	//If we have anything in info queue, it will also be displayed
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* GraphicsSetup::HRException::GetType() const noexcept
{
	return "Infinum GraphicsSetup Exception";
}

HRESULT GraphicsSetup::HRException::GetErrorCode() const noexcept
{
	return hr;
}

std::string GraphicsSetup::HRException::GetErrorString() const noexcept
{
	return DXGetErrorString(hr);
}

std::string GraphicsSetup::HRException::GetErrorDescription() const noexcept
{
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string GraphicsSetup::HRException::GetErrorInfo() const noexcept
{
	return info;
}


const char* GraphicsSetup::DeviceRemovedException::GetType() const noexcept
{
	return "Infinum GraphicsSetup Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

GraphicsSetup::InfoOnlyException::InfoOnlyException(int line, const char * file, std::vector<std::string> infoMsgs) noexcept
	:
	Exception(line, file)
	
{
	// join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		info += m;
		info.push_back('\n');
	}
	// remove final newline if exists
	if (!info.empty())
	{
		info.pop_back();
	}
}

const char* GraphicsSetup::InfoOnlyException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl;
	//If we have anything in info queue, it will also be displayed
	if (!info.empty())
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	oss << GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* GraphicsSetup::InfoOnlyException::GetType() const noexcept
{
	return "Infinum GraphicsSetup Exception";
}

std::string GraphicsSetup::InfoOnlyException::GetErrorInfo() const noexcept
{
	return info;
}
