#include "GraphicsSetup.h"
#include "dxerr.h"
#include<sstream>
#include<d3dcompiler.h>
#include<cmath>
#include<DirectXMath.h>
#include "GraphicsSetupThrowsMacro.h"

#pragma comment(lib,"D3DCompiler.lib")	//Can be used to compile shaders at runtime; but we will be using for shader loading function
#pragma comment(lib,"d3d11.lib")



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

	//We are going to create and bind the z-buffer.
	//Depth buffer is kind of like a frame buffer. Difference being that the frame buffer stores colours,
	//while depth buffer stores depth values.
	//When we create swap chain, frame buffer is created and binded automatically.
	//But for depth buffer, we need to create the texture ourselves and then bind it to the output merger.
	//So first, we need to create and bind the state. Then, we need to create and bind the texture.

	D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {}; //We will be only using Depth Buffer. Since depth buffer and stencil buffer share a common space, so STENCIL keyword appears here.
	DepthStencilDesc.DepthEnable = TRUE;
	DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS; //This means that anything whose value will be less on z axis (in our case), that pixel will override what was previously written
	wrl::ComPtr<ID3D11DepthStencilState>pDepthStencilState;
	GFX_THROW_INFO(pDevice->CreateDepthStencilState(&DepthStencilDesc, &pDepthStencilState));

	//Now we bind depth state
	pContext->OMSetDepthStencilState(pDepthStencilState.Get(), 1u);//Second parameter won't matter since it is used for stencil stuff.

	//Step 2-> Create Depth Stencil Texture
	wrl::ComPtr<ID3D11Texture2D> pDepthStencilTexture;
	D3D11_TEXTURE2D_DESC DepthStencilTextureDesc = {};
	//Width and height should match swap chain vals
	DepthStencilTextureDesc.Width = 800u;
	DepthStencilTextureDesc.Height = 600u;
	DepthStencilTextureDesc.MipLevels = 1u;	//We need a single mipmap. (To study later)
	DepthStencilTextureDesc.ArraySize = 1u;	//We need a single element array of texture. (We can create multiple element array though)
	DepthStencilTextureDesc.Format = DXGI_FORMAT_D32_FLOAT; //D32 is a special 32 bit floating point value provided for depth.
	//Below stuffs are for anti-aliasing
	DepthStencilTextureDesc.SampleDesc.Count = 1u;
	DepthStencilTextureDesc.SampleDesc.Quality = 0u;
	DepthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(pDevice->CreateTexture2D(&DepthStencilTextureDesc, nullptr, &pDepthStencilTexture));

	//Create Depth Stencil Texture View
	D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc = {};
	DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DepthStencilViewDesc.Texture2D.MipSlice = 0u;
	GFX_THROW_INFO(pDevice->CreateDepthStencilView(pDepthStencilTexture.Get(), &DepthStencilViewDesc, &pDepthStencilView));

	//Now we bind depth stencil view to Output merger->
	pContext->OMSetRenderTargets(1u, pTarget.GetAddressOf(), pDepthStencilView.Get());

	//We need to configure the viewport for rasterizing
	D3D11_VIEWPORT vport;
	vport.Width = 800.0f;
	vport.Height = 600.0f;
	vport.MinDepth = 0.0f;
	vport.MaxDepth = 1.0f;
	vport.TopLeftX = 0.0f;
	vport.TopLeftY = 0.0f;
	pContext->RSSetViewports(1u, &vport);
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
		//Every time we call a new frame, we also need to clear the depth buffer. Doing it now below
		pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
		//Since the maximum depth of our system is 1.0f, we set the third parameter as 1.0f
	
}

/*This function is being edited to display a cube having solid colours on each of its face. To do that, we will
generate a triangle ID for each triangle and use that to look into an array of colours, which will then determine which triangle
will have which colour.*/
void GraphicsSetup::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY(pContext->DrawIndexed(count, 0u, 0u));
}

void GraphicsSetup::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	projection = proj;
}

DirectX::XMMATRIX GraphicsSetup::GetProjection() const noexcept
{
	return projection;
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
