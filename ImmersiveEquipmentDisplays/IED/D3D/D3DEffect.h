#pragma once

#include "D3DEffectResources.h"
#include "D3DHelpers.h"

#include "Common/VectorMath.h"

/*namespace IED
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

		[[nodiscard]] inline constexpr auto XM_CALLCONV GetWorldMatrix() const noexcept
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
}*/

namespace IED
{
	struct D3DEffectConstants
	{
		DirectX::XMVECTOR diffuseColor;
		DirectX::XMVECTOR emissiveColor;
		DirectX::XMVECTOR specularColorAndPower{ 1, 1, 1, 16 };

		DirectX::XMVECTOR lightDirection[3];
		DirectX::XMVECTOR lightDiffuseColor[3];
		DirectX::XMVECTOR lightSpecularColor[3];

		DirectX::XMVECTOR eyePosition;

		DirectX::XMVECTOR fogColor{ DirectX::g_XMZero.v };
		DirectX::XMVECTOR fogVector{ DirectX::g_XMZero.v };

		DirectX::XMMATRIX world;
		DirectX::XMVECTOR worldInverseTranspose[3];
		DirectX::XMMATRIX worldViewProj;
	};

	static_assert((sizeof(D3DEffectConstants) % 16) == 0, "CB size not padded correctly");

	class D3DEffectResources;

	enum class D3DEffectTestDirtyFlags : std::uint32_t
	{
		kNone = 0,

		kMaterial       = 1u << 0,
		kLightColor     = 1u << 1,
		kLightDirection = 1u << 2,

		kLight = kLightColor | kLightDirection,

		kAll = static_cast<std::uint32_t>(-1)
	};

	DEFINE_ENUM_CLASS_BITWISE(D3DEffectTestDirtyFlags);

	class D3DEffect
	{
	public:
		D3DEffect(
			ID3D11Device*     a_device,
			D3DVertexShaderID a_vertexShader,
			D3DPixelShaderID  a_pixelShader) noexcept(false);

		void             UpdateWorldMatrix(const NiTransform& a_transform);
		void             UpdateWorldMatrix(const NiAVObject* a_object);
		void XM_CALLCONV UpdateWorldMatrix(DirectX::XMMATRIX a_matrix);

		void XM_CALLCONV SetWorldPosition(DirectX::XMVECTOR a_pos);

		void SetMatrices(
			const DirectX::XMMATRIX& a_view,
			const DirectX::XMMATRIX& a_projection);

		void ApplyEffect(ID3D11DeviceContext* a_context, const D3DEffectResources& a_resources);

		[[nodiscard]] inline constexpr auto XM_CALLCONV GetWorldMatrix() const noexcept
		{
			return m_world;
		}

		[[nodiscard]] inline constexpr auto XM_CALLCONV GetWorldPosition() const noexcept
		{
			sizeof(D3DEffect);
			return m_world.r[3];
		}

		void XM_CALLCONV SetDiffuseColor(DirectX::XMVECTOR a_color);
		void XM_CALLCONV SetEmissiveColor(DirectX::XMVECTOR a_color);
		void XM_CALLCONV SetSpecularColor(DirectX::XMVECTOR a_color);

		void SetSpecularPower(float a_value);

		void             SetLightingEnabled(bool a_switch);
		void XM_CALLCONV SetAmbientLightColor(DirectX::FXMVECTOR a_color);

		void SetAlpha(float a_alpha);

	protected:
		void UpdateConstants();

		D3DEffectConstants m_constants;

		DirectX::XMMATRIX m_world{ VectorMath::g_identity };
		DirectX::XMMATRIX m_view{ VectorMath::g_identity };
		DirectX::XMMATRIX m_proj{ VectorMath::g_identity };

		DirectX::XMVECTOR m_emissiveColor{ DirectX::g_XMZero.v };
		DirectX::XMVECTOR m_ambientLightColor{ 1, 1, 1, 16 };

		DirectX::XMVECTOR m_diffuseColor{ DirectX::g_XMOne.v };

		DirectX::XMVECTOR m_lightDirection[3];
		DirectX::XMVECTOR m_lightDiffuseColor[3];
		DirectX::XMVECTOR m_lightSpecularColor[3];

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;

		float m_alpha{ 1.0f };
		bool  m_lightingEnabled{ false };

		stl::flag<D3DEffectTestDirtyFlags> m_dirtyFlags{ D3DEffectTestDirtyFlags::kAll };

		D3DVertexShaderID m_vertexShader;
		D3DPixelShaderID  m_pixelShader;
	};

}