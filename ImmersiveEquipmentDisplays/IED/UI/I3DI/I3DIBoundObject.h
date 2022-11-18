#pragma once

#include "I3DIObject.h"

#include "I3DIBoundingBase.h"

namespace IED
{
	namespace UI
	{
		struct I3DICommonData;

		class I3DIBoundObject :
			public I3DIObject
		{
		public:
			I3DIBoundObject(
				BoundingShape a_type = BoundingShape::kOrientedBox) noexcept(false);

			virtual ~I3DIBoundObject() noexcept = default;

			virtual I3DIBoundObject* AsBoundObject() override
			{
				return this;
			};

			virtual void UpdateBound() = 0;

			virtual bool ObjectIntersects(
				I3DICommonData& a_data,
				const I3DIRay&  a_ray,
				float&          a_dist) override;

			void XM_CALLCONV DrawBoundingShape(
				D3DPrimitiveBatch& a_batch,
				DirectX::XMVECTOR  a_color) const;

			[[nodiscard]] bool XM_CALLCONV Intersects(
				DirectX::XMVECTOR a_origin,
				DirectX::XMVECTOR a_direction,
				float&            a_distance) const;

			[[nodiscard]] float XM_CALLCONV GetCenterDistance(
				DirectX::XMVECTOR a_origin) const;

			[[nodiscard]] float XM_CALLCONV GetCenterDistanceSq(
				DirectX::XMVECTOR a_origin) const;

			[[nodiscard]] DirectX::XMVECTOR XM_CALLCONV GetBoundingShapeCenter() const;

			template <class T>
			[[nodiscard]] inline constexpr T* GetBoundingShape() const noexcept //
				requires(std::is_base_of_v<I3DIBoundingBase, T>)
			{
				if (T::BOUND_TYPE == m_type)
				{
					return static_cast<T*>(m_bound.get());
				}
				else
				{
					return nullptr;
				}
			}

			[[nodiscard]] inline constexpr auto GetBoundingShapeType() const noexcept
			{
				return m_type;
			}

		private:
			std::unique_ptr<I3DIBoundingBase> m_bound;
			BoundingShape                     m_type;
		};
	}
}