#pragma once

#include "ConstantBuffer.h"
#include "Drawable.h"
#include <DirectXMath.h>

class TransformConstantBuffer : public Bindable
{
public:
	TransformConstantBuffer(GraphicsSetup& gfx, const Drawable& parent);
	void Bind(GraphicsSetup& gfx) noexcept override;
private:
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf;
	const Drawable& parent;
};
