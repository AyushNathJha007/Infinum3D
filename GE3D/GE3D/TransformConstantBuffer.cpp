#include "TransformConstantBuffer.h"

TransformConstantBuffer::TransformConstantBuffer(GraphicsSetup& gfx, const Drawable& parent)
	:
	vcbuf(gfx),
	parent(parent)
{}

void TransformConstantBuffer::Bind(GraphicsSetup& gfx) noexcept
{
	vcbuf.Update(gfx,
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * gfx.GetProjection()
		)
	);
	vcbuf.Bind(gfx);
}