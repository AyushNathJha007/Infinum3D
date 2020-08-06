#include "InputLayout.h"
#include "GraphicsSetupThrowsMacro.h"

InputLayout::InputLayout(GraphicsSetup& gfx,
	const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout,
	ID3DBlob* pVertexShaderBytecode)
{
	INFOMAN(gfx);
	HRESULT HR;
	GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
		layout.data(), (UINT)layout.size(),
		pVertexShaderBytecode->GetBufferPointer(),
		pVertexShaderBytecode->GetBufferSize(),
		&pInputLayout
	));
}

void InputLayout::Bind(GraphicsSetup& gfx) noexcept
{
	GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
}