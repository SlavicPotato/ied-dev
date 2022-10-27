#pragma once

#include "D3DBoundingOrientedBox.h"
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
	};

	DEFINE_ENUM_CLASS_BITWISE(D3DObjectFlags);

	struct D3DObjectFlagsBF
	{
		D3DObjectRasterizerState rasterizerState: 1;
		std::uint32_t            none           : 30;
	};
	static_assert(sizeof(D3DObjectFlagsBF) == sizeof(D3DObjectFlags));

	class D3DObject :
		public D3DEffect
	{
	public:
		D3DObject(
			ID3D11Device*                        a_device,
			const std::shared_ptr<D3DModelData>& a_data);

		void UpdateBound();

		[[nodiscard]] bool XM_CALLCONV Intersects(
			DirectX::FXMVECTOR a_origin,
			DirectX::FXMVECTOR a_direction,
			float&             a_distance) const;

		[[nodiscard]] float XM_CALLCONV GetDistance(
			DirectX::FXMVECTOR a_origin) const;

		[[nodiscard]] float XM_CALLCONV GetDistanceSq(
			DirectX::FXMVECTOR a_origin) const;

		[[nodiscard]] float XM_CALLCONV GetCenterDistance(
			DirectX::FXMVECTOR a_origin) const;

		[[nodiscard]] float XM_CALLCONV GetCenterDistanceSq(
			DirectX::FXMVECTOR a_origin) const;

		void Draw(D3DCommon& a_scene);

		[[nodiscard]] inline constexpr auto& GetBound() const noexcept
		{
			return m_bound;
		}

		[[nodiscard]] inline constexpr auto& GetModelData() const noexcept
		{
			return m_data;
		}

		/*[[nodiscard]] inline constexpr auto& GetEffect() const noexcept
		{
			return m_effect;
		}*/

		[[nodiscard]] inline constexpr void SetRasterizerState(
			D3DObjectRasterizerState a_value) noexcept
		{
			m_flagsbf.rasterizerState = a_value;
		}

		[[nodiscard]] inline constexpr void EnableDepth(bool a_switch) noexcept
		{
			m_flags.set(D3DObjectFlags::kDepth, a_switch);
		}

		[[nodiscard]] inline constexpr bool IsOpaque() const noexcept
		{
			return m_alpha >= 1.0f;
		}

	private:
		D3DBoundingOrientedBox m_bound;

		std::shared_ptr<D3DModelData> m_data;

		union
		{
			stl::flag<D3DObjectFlags> m_flags{ D3DObjectFlags::kNone };
			D3DObjectFlagsBF          m_flagsbf;
		};
	};
}