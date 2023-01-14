#pragma once

#include "D3DBoundingOrientedBox.h"
#include "D3DBoundingSphere.h"

namespace IED
{
	struct ModelData;

	class D3DModelData
	{
	public:
		D3DModelData(
			ID3D11Device*                     a_device,
			const std::shared_ptr<ModelData>& a_data) noexcept(false);

		void Draw(ID3D11DeviceContext* a_context);

		[[nodiscard]] constexpr auto& GetModelData() const noexcept
		{
			return m_modelData;
		}

		[[nodiscard]] constexpr auto& GetBoundingOrientedBox() const noexcept
		{
			return m_boundingOrientedBox;
		}

		[[nodiscard]] constexpr auto& GetBoundingSphere() const noexcept
		{
			return m_boundingSphere;
		}

		[[nodiscard]] bool HasVertexColors() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

		std::shared_ptr<ModelData> m_modelData;

		D3DBoundingOrientedBox m_boundingOrientedBox;
		D3DBoundingSphere      m_boundingSphere;

		UINT m_indexCount{ 0 };
	};
}