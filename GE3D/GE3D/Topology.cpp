#include "Topology.h"

Topology::Topology(GraphicsSetup& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	:
	type(type)
{}

void Topology::Bind(GraphicsSetup& gfx) noexcept
{
	GetContext(gfx)->IASetPrimitiveTopology(type);
}