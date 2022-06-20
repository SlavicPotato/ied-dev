#include "pch.h"

#include "D3DModelData.h"

#include "D3DAssets.h"
#include "D3DHelpers.h"

namespace IED
{
	using namespace DirectX;

	D3DModelData::D3DModelData(
		ID3D11Device*                     a_device,
		const std::shared_ptr<ModelData>& a_data) noexcept(false) :
		m_modelData(a_data),
		m_bound(a_data->CreateBoundingBox())
	{
		assert(a_device);

		auto vsize = static_cast<UINT>(a_data->vertices.size() * sizeof(VertexPositionNormalColor));
		auto isize = static_cast<UINT>(a_data->indices.size() * sizeof(std::uint16_t));

		CD3D11_BUFFER_DESC vdesc(vsize, D3D11_BIND_VERTEX_BUFFER);
		CD3D11_BUFFER_DESC idesc(isize, D3D11_BIND_INDEX_BUFFER);

		D3D11_SUBRESOURCE_DATA initData = { a_data->vertices.data(), vsize, 0 };

		ThrowIfFailed(a_device->CreateBuffer(
			std::addressof(vdesc),
			std::addressof(initData),
			m_vertexBuffer.ReleaseAndGetAddressOf()));

		initData = { a_data->indices.data(), isize, 0 };

		ThrowIfFailed(a_device->CreateBuffer(
			std::addressof(idesc),
			std::addressof(initData),
			m_indexBuffer.ReleaseAndGetAddressOf()));

		m_indexCount = static_cast<UINT>(a_data->indices.size());
	}

	void D3DModelData::Draw(ID3D11DeviceContext* a_context)
	{
		assert(a_context);
		assert(m_vertexBuffer.Get());

		auto vb           = m_vertexBuffer.Get();
		UINT vertexStride = sizeof(VertexPositionNormalColor);
		UINT vertexOffset = 0;

		a_context->IASetVertexBuffers(
			0,
			1,
			std::addressof(vb),
			std::addressof(vertexStride),
			std::addressof(vertexOffset));

		a_context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		a_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		a_context->DrawIndexed(m_indexCount, 0, 0);
	}

	bool D3DModelData::HasVertexColors() const noexcept
	{
		return m_modelData->m_hasVertexColors;
	}

}