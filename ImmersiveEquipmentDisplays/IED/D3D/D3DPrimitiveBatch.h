#pragma once

#include "D3DCommon.h"
#include "D3DEffect.h"
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

		template <class... Args>
		constexpr void AddLine(Args&&... a_args)
		{
			m_lines.emplace_back(std::forward<Args>(a_args)...);
		}

		template <class... Args>
		constexpr void AddTriangle(Args&&... a_args)
		{
			m_tris.emplace_back(std::forward<Args>(a_args)...);
		}

		constexpr void EnableDepth(bool a_switch) noexcept
		{
			m_flags.set(D3DPrimitiveBatchFlags::kDepth, a_switch);
		}

	private:
		std::unique_ptr<DirectX::PrimitiveBatch<VertexPositionColorAV>> m_batch;

		struct vertex_pair_type
		{
			constexpr vertex_pair_type(
				const DirectX::XMVECTOR& a_p1,
				const DirectX::XMVECTOR& a_c1,
				const DirectX::XMVECTOR& a_p2,
				const DirectX::XMVECTOR& a_c2) noexcept :
				first(a_p1, a_c1),
				second(a_p2, a_c2)
			{
			}

			constexpr vertex_pair_type(
				const DirectX::XMVECTOR& a_p1,
				const DirectX::XMVECTOR& a_p2,
				const DirectX::XMVECTOR& a_c) noexcept :
				first(a_p1, a_c),
				second(a_p2, a_c)
			{
			}

			VertexPositionColorAV first;
			VertexPositionColorAV second;
		};

		struct triangle_type
		{
			constexpr triangle_type(
				const DirectX::XMVECTOR& a_p1,
				const DirectX::XMVECTOR& a_p2,
				const DirectX::XMVECTOR& a_p3,
				const DirectX::XMVECTOR& a_c) noexcept :
				first(a_p1, a_c),
				second(a_p2, a_c),
				third(a_p3, a_c)
			{
			}

			VertexPositionColorAV first;
			VertexPositionColorAV second;
			VertexPositionColorAV third;
		};

		stl::vector<vertex_pair_type> m_lines;

		stl::vector<triangle_type> m_tris;

		stl::flag<D3DPrimitiveBatchFlags> m_flags{ D3DPrimitiveBatchFlags::kNone };
	};
}