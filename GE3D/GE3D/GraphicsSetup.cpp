#include "GraphicsSetup.h"
#include "dxerr.h"
#include<sstream>

#pragma comment(lib,"d3d11.lib")

//Additional Macros for handling GraphicsSetup exception throw stuff (Now also includes DXGI Info)
#define GFX_EXCEPT_NOINFO(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( HR = (hrcall) ) ) throw Graphics::HRException( __LINE__,__FILE__,HR )

#ifndef NDEBUG	//Additional info messages will be shown, when in debug mode. Else, additional info won't be shown.
#define GFX_EXCEPT(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( HR = (hrcall) ) ) throw GFX_EXCEPT(HR)
#define GFX_DEVICE_REMOVED_EXCEPT(HR) GraphicsSetup::DeviceRemovedException( __LINE__,__FILE__,(HR),infoManager.GetMessages() )
#else
#define GFX_EXCEPT(HR) GraphicsSetup::HRException( __LINE__,__FILE__,(HR) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(HR) GraphicsSetup::DeviceRemovedException(__LINE__,__FILE__,(HR))
#endif

namespace wrl = Microsoft::WRL;
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
