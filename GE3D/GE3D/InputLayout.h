#pragma once
#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(GraphicsSetup& gfx,
		const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		ID3DBlob* pVertexShaderBytecode);
	void Bind(GraphicsSetup& gfx) noexcept override;
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
};