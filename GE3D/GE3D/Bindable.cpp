#include "Bindable.h"

ID3D11DeviceContext* Bindable::GetContext(GraphicsSetup& gfx) noexcept
{
	return gfx.pContext.Get();
}

ID3D11Device* Bindable::GetDevice(GraphicsSetup& gfx) noexcept
{
	return gfx.pDevice.Get();
}

DxgiInfoManager& Bindable::GetInfoManager(GraphicsSetup& gfx) noexcept(!IS_DEBUG)
{
#ifndef NDEBUG
	return gfx.infoManager;
#else
	throw std::logic_error("Can't Access gfx.InfoManager in Release Configuration.");
#endif
}