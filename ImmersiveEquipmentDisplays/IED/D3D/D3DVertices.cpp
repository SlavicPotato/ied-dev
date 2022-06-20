#include "pch.h"

#include "D3DVertices.h"

namespace IED
{
	const D3D11_INPUT_ELEMENT_DESC VertexPositionColorAV::InputElements[] = {
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	static_assert(sizeof(VertexPositionColorAV) == 32, "Vertex struct/layout mismatch");
}