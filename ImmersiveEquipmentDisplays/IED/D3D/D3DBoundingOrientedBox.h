#pragma once

namespace IED
{
	class D3DPrimitiveBatch;
	class D3DCommon;

	class D3DBoundingOrientedBox :
		public DirectX::BoundingOrientedBox
	{
	public:
		using DirectX::BoundingOrientedBox::BoundingOrientedBox;

		void XM_CALLCONV DrawBox(
			D3DPrimitiveBatch& a_batch,
			DirectX::XMVECTOR  a_color) const;

		[[nodiscard]] DirectX::XMVECTOR XM_CALLCONV GetPoint(DirectX::XMVECTOR a_direction) const;

		[[nodiscard]] SKMP_FORCEINLINE static DirectX::XMVECTOR XM_CALLCONV D3DBoundingOrientedBox::GetPoint(
			DirectX::XMVECTOR a_direction,
			DirectX::XMVECTOR a_center,
			DirectX::XMVECTOR a_extents,
			DirectX::XMVECTOR a_orientation)
		{
			return DirectX::XMVectorAdd(
				DirectX::XMVector3Rotate(
					DirectX::XMVectorMultiply(a_extents, a_direction),
					a_orientation),
				a_center);
		};

		[[nodiscard]] DirectX::XMVECTOR XM_CALLCONV GetPopupAnchorPoint(
			D3DCommon&         a_scene,
			DirectX::XMVECTOR  a_direction,
			float              a_offset,
			DirectX::XMVECTOR& a_origin) const;
	};
}