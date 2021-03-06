#pragma once
#include "ANJWIN.h"
#include "ANJException.h"
#include<d3d11.h>
#include <vector>
#include "DxgiInfoManager.h"
#include<wrl.h>	//This is where ComPtr resides
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>

class GraphicsSetup
{
	friend class Bindable;
public:
	class Exception : public ANJException //Base GraphicsSetup Exception
	{
		using ANJException::ANJException;
	};
	class HRException :public Exception //Exception involving HRESULT, since the output of most parameters used in D3D is HRESULT
	{
	public:
		HRException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept; //It now takes the info messages as extra parameter
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept; //It will retrieve the error info, which gives extra description about the error
	private:
		HRESULT hr;
		std::string info; //Stores the string containing error info
	};
	class InfoOnlyException :public Exception
	{
	public:
		InfoOnlyException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;	//It only stores info, no HRESULT like previous exception classes
	};
	class DeviceRemovedException :public HRException //Specialised Exception -> Returns errors like GPU physically removed, driver crash,etc
	{
		using HRException::HRException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
	GraphicsSetup(HWND hWnd);
	GraphicsSetup(const GraphicsSetup&) = delete;
	GraphicsSetup& operator=(const GraphicsSetup&) = delete;

	~GraphicsSetup()=default;
	void EndFrame(); //This will perform the operation of presenting Back Buffer To Front Buffer(known as flipping)
	void ClearBuffer(float red, float green, float blue) noexcept;
	//void DrawTriangleTest(float angle, float x, float y);
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	
private:
#ifndef NDEBUG	//DxgiInfoManager will be used, only when we are in debug mode. In production/release, it won't be used
	DxgiInfoManager infoManager;
#endif
	//All previous declarations exchanged with ComPtr
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;	//Used for allocating the device(setting it up) and creating stuff
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext; //Used for configuring a pipeline in exceuting or issuing rendering commands
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView;	//In order to bind texture to the pipeline, we need to get a view on the texture
	DirectX::XMMATRIX projection;
};
