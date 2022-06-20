#pragma once

#include "D3DCommon.h"
#include "D3DEffect.h"

namespace IED
{
	class D3DModelData;

	enum class D3DObjectFlags : std::uint32_t
	{
		kNone = 0,

		kWireframe = 1u << 0,
		kDepth     = 1u << 1,

		kOverrideDepth = 1u << 16,
	};

	DEFINE_ENUM_CLASS_BITWISE(D3DObjectFlags);

	struct D3DObjectFlagsBF
	{
		D3DObjectRasterizerState rasterizerState: 1;
		std::uint32_t            none           : 30;
	};
	static_assert(sizeof(D3DObjectFlagsBF) == sizeof(D3DObjectFlags));

	class D3DCommon;

	class D3DObject :
		public D3DEffect<DirectX::VertexPositionNormalColor>
	{
	public:
		D3DObject(
			ID3D11Device*                        a_device,
			const std::shared_ptr<D3DModelData>& a_data) noexcept(false);

		void UpdateBound();

		[[nodiscard]] bool XM_CALLCONV Intersects(
			DirectX::FXMVECTOR a_origin,
			DirectX::FXMVECTOR a_direction,
			float&             a_distance) const;

		void Draw(D3DCommon& a_scene);

		void Draw(
			D3DCommon&               a_scene,
			const DirectX::XMMATRIX& a_view,
			const DirectX::XMMATRIX& a_projection);

		[[nodiscard]] inline constexpr auto& GetBound() const noexcept
		{
			return m_bound;
		}

		[[nodiscard]] inline constexpr auto& GetModelData() const noexcept
		{
			return m_data;
		}

		[[nodiscard]] inline constexpr auto& GetEffect() const noexcept
		{
			return m_effect;
		}

		[[nodiscard]] inline constexpr void SetRasterizerState(
			D3DObjectRasterizerState a_value) noexcept
		{
			m_flagsbf.rasterizerState = a_value;
		}
		
		[[nodiscard]] inline constexpr void SetOverrideDepth(bool a_switch) noexcept
		{
			m_flags.set(D3DObjectFlags::kDepth, a_switch);
			m_flags.set(D3DObjectFlags::kOverrideDepth);
		}
		
		[[nodiscard]] inline constexpr void ClearOverrideDepth(bool a_switch) noexcept
		{
			m_flags.clear(D3DObjectFlags::kOverrideDepth);
		}

	private:
		DirectX::BoundingOrientedBox m_bound;

		std::shared_ptr<D3DModelData> m_data;

		union
		{
			stl::flag<D3DObjectFlags> m_flags{ D3DObjectFlags::kNone };
			D3DObjectFlagsBF          m_flagsbf;
		};
	};
}