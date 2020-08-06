#pragma once

#include "GraphicsSetup.h"

class Bindable
{
public:
	virtual void Bind(GraphicsSetup &gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	static ID3D11DeviceContext* GetContext(GraphicsSetup& gfx) noexcept;
	static ID3D11Device* GetDevice(GraphicsSetup& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(GraphicsSetup& gfx) noexcept(!IS_DEBUG);

};