#pragma once
#include "Bindable.h"

class Topology : public Bindable
{
public:
	Topology(GraphicsSetup& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
	void Bind(GraphicsSetup& gfx) noexcept override;
protected:
	D3D11_PRIMITIVE_TOPOLOGY type;
};
