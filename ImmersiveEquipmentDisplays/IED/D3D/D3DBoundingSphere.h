#pragma once

namespace IED
{
	class D3DPrimitiveBatch;
	class D3DCommon;

	class D3DBoundingSphere :
		public DirectX::BoundingSphere
	{
	public:
		using DirectX::BoundingSphere::BoundingSphere;

		void XM_CALLCONV DrawSphere(
			D3DPrimitiveBatch& a_batch,
			DirectX::XMVECTOR  a_color) const;

	};
}