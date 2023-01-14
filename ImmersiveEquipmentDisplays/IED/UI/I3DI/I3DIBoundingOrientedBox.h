#pragma once

#include "I3DIBoundingBase.h"

#include "IED/D3D/D3DBoundingOrientedBox.h"

namespace IED
{
	namespace UI
	{
		class I3DIBoundingOrientedBox :
			public I3DIBoundingBase
		{
		public:
			static constexpr auto BOUND_TYPE = BoundingShape::kOrientedBox;

			virtual ~I3DIBoundingOrientedBox() noexcept override = default;

			virtual void XM_CALLCONV Draw(
				D3DPrimitiveBatch& a_batch,
				DirectX::XMVECTOR  a_color) const override;

			virtual bool XM_CALLCONV Intersects(
				DirectX::XMVECTOR a_origin,
				DirectX::XMVECTOR a_direction,
				float&            a_distance) const override;

			[[nodiscard]] virtual float XM_CALLCONV GetCenterDistance(
				DirectX::XMVECTOR a_origin) const;

			[[nodiscard]] virtual float XM_CALLCONV GetCenterDistanceSq(
				DirectX::XMVECTOR a_origin) const;

			[[nodiscard]] virtual DirectX::XMVECTOR XM_CALLCONV GetBoundingShapeCenter() const override;

			[[nodiscard]] constexpr auto& GetBound() const noexcept
			{
				return m_bound;
			}
			
			[[nodiscard]] constexpr auto& GetBound() noexcept
			{
				return m_bound;
			}

		protected:
			D3DBoundingOrientedBox m_bound;
		};
	}
}