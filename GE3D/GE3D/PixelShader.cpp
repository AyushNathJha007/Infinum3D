#include "PixelShader.h"
#include "GraphicsSetupThrowsMacro.h"

PixelShader::PixelShader(GraphicsSetup& gfx, const std::wstring& path)
{
	INFOMAN(gfx);
	HRESULT HR;
	Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
}

void PixelShader::Bind(GraphicsSetup& gfx) noexcept
{
	GetContext(gfx)->PSSetShader(pPixelShader.Get(), nullptr, 0u);
}