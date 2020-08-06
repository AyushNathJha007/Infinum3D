#pragma once
#include "Bindable.h"

class PixelShader : public Bindable
{
public:
	PixelShader(GraphicsSetup& gfx, const std::wstring& path);
	void Bind(GraphicsSetup& gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};
