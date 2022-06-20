#pragma once

#include "D3DHelpers.h"

#include "Common/VectorMath.h"

namespace IED
{
	template <class VT>
	class D3DEffect
	{
	public:
		D3DEffect(ID3D11Device* a_device) noexcept(false);

		void             UpdateWorldMatrix(const NiTransform& a_transform);
		void             UpdateWorldMatrix(const NiAVObject* a_object);
		void XM_CALLCONV UpdateWorldMatrix(DirectX::XMMATRIX a_matrix);

		void SetMatrices(
			const DirectX::XMMATRIX& a_view,
			const DirectX::XMMATRIX& a_projection);

		void ApplyEffect(ID3D11DeviceContext* a_context);

		[[nodiscard]] inline constexpr auto& GetWorldMatrix() const noexcept
		{
			return m_world;
		}

		[[nodiscard]] inline constexpr auto& GetEffect() const noexcept
		{
			return m_effect;
		}

	protected:

		void CreateInputLayout(ID3D11Device* a_device) noexcept(false);

		DirectX::XMMATRIX m_world{ DirectX::SimpleMath::Matrix::Identity };

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
		std::unique_ptr<DirectX::BasicEffect>     m_effect;
	};

	template <class VT>
	D3DEffect<VT>::D3DEffect(ID3D11Device* a_device) noexcept(false) :
		m_effect(std::make_unique<BasicEffect>(a_device))
	{
	}

	template <class VT>
	void D3DEffect<VT>::UpdateWorldMatrix(const NiTransform& a_transform)
	{
		m_world = VectorMath::NiTransformTo4x4Matrix(a_transform);
	}

	template <class VT>
	void D3DEffect<VT>::UpdateWorldMatrix(const NiAVObject* a_object)
	{
		m_world = VectorMath::NiTransformTo4x4Matrix(a_object->m_worldTransform);
	}

	template <class VT>
	void XM_CALLCONV D3DEffect<VT>::UpdateWorldMatrix(DirectX::XMMATRIX a_matrix)
	{
		m_world = a_matrix;
	}

	template <class VT>
	void D3DEffect<VT>::SetMatrices(
		const DirectX::XMMATRIX& a_view,
		const DirectX::XMMATRIX& a_projection)
	{
		m_effect->SetMatrices(m_world, a_view, a_projection);
	}

	template <class VT>
	void D3DEffect<VT>::ApplyEffect(ID3D11DeviceContext* a_context)
	{
		a_context->IASetInputLayout(m_inputLayout.Get());
		m_effect->Apply(a_context);
	}

	template <class VT>
	void D3DEffect<VT>::CreateInputLayout(ID3D11Device* a_device) noexcept(false)
	{
		ThrowIfFailed(DirectX::CreateInputLayoutFromEffect<VT>(
			a_device,
			m_effect.get(),
			m_inputLayout.ReleaseAndGetAddressOf()));
	}
}