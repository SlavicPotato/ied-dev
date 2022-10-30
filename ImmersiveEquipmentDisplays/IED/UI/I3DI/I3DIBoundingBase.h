#pragma once

namespace IED
{
	class D3DPrimitiveBatch;

	namespace UI
	{
		enum class BoundingShape
		{
			kInvalid = -1,

			kOrientedBox = 0,
			kSphere      = 1
		};

		class I3DIBoundingBase
		{
		public:
			inline static constexpr auto BOUND_TYPE = BoundingShape::kInvalid;

			virtual ~I3DIBoundingBase() noexcept = default;

			virtual void XM_CALLCONV Draw(
				D3DPrimitiveBatch& a_batch,
				DirectX::XMVECTOR  a_color) const {};

			// ray
			[[nodiscard]] virtual bool XM_CALLCONV Intersects(
				DirectX::XMVECTOR a_origin,
				DirectX::XMVECTOR a_direction,
				float&            a_distance) const = 0;

			[[nodiscard]] virtual float XM_CALLCONV GetCenterDistance(
				DirectX::XMVECTOR a_origin) const = 0;

			[[nodiscard]] virtual float XM_CALLCONV GetCenterDistanceSq(
				DirectX::XMVECTOR a_origin) const = 0;

			[[nodiscard]] virtual DirectX::XMVECTOR XM_CALLCONV GetBoundingShapeCenter() const = 0;
		};
	}
}