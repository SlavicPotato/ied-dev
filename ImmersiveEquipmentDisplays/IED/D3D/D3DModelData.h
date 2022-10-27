#pragma once

#include "D3DBoundingOrientedBox.h"

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

		[[nodiscard]] inline constexpr auto& GetModelData() const noexcept
		{
			return m_modelData;
		}

		[[nodiscard]] inline constexpr auto& GetBound() const noexcept
		{
			return m_bound;
		}
		
		[[nodiscard]]  bool HasVertexColors() const noexcept;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

		std::shared_ptr<ModelData> m_modelData;

		D3DBoundingOrientedBox m_bound;

		UINT m_indexCount{ 0 };
	};
}