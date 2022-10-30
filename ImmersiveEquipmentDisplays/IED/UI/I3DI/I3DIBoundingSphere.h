#pragma once

#include "I3DIBoundingBase.h"

#include "IED/D3D/D3DBoundingSphere.h"

namespace IED
{
	namespace UI
	{
		class I3DIBoundingSphere :
			public I3DIBoundingBase
		{
		public:
			inline static constexpr auto BOUND_TYPE = BoundingShape::kSphere;

			virtual ~I3DIBoundingSphere() noexcept override = default;

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

			[[nodiscard]] inline constexpr auto& GetBound() const noexcept
			{
				return m_bound;
			}

			[[nodiscard]] inline constexpr auto& GetBound() noexcept
			{
				return m_bound;
			}

		protected:
			D3DBoundingSphere m_bound;
		};
	}
}