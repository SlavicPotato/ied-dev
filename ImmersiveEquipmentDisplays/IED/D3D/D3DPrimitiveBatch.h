#pragma once

#include "D3DEffect.h"
#include "D3DCommon.h"
#include "D3DVertices.h"

namespace IED
{
	enum class D3DPrimitiveBatchFlags : std::uint32_t
	{
		kNone = 0,

		kDepth = 1u << 0,
	};

	DEFINE_ENUM_CLASS_BITWISE(D3DPrimitiveBatchFlags);

	class D3DPrimitiveBatch :
		public D3DEffect
	{
	public:
		D3DPrimitiveBatch(
			ID3D11Device*        a_device,
			ID3D11DeviceContext* a_context) noexcept(false);

		void Draw(
			D3DCommon& a_scene);

		void XM_CALLCONV AddLine(
			DirectX::XMVECTOR a_p1,
			DirectX::XMVECTOR a_c1,
			DirectX::XMVECTOR a_p2,
			DirectX::XMVECTOR a_c2);

		void XM_CALLCONV AddLine(
			DirectX::XMVECTOR a_p1,
			DirectX::XMVECTOR a_p2,
			DirectX::XMVECTOR a_c);

		inline constexpr void EnableDepth(bool a_switch) noexcept
		{
			m_flags.set(D3DPrimitiveBatchFlags::kDepth, a_switch);
		}

	private:
		std::unique_ptr<DirectX::PrimitiveBatch<VertexPositionColorAV>> m_batch;

		stl::vector<std::pair<VertexPositionColorAV, VertexPositionColorAV>> m_lines;

		stl::flag<D3DPrimitiveBatchFlags> m_flags{ D3DPrimitiveBatchFlags::kNone };
	};
}