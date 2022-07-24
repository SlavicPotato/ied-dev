#include "pch.h"

#include "D3DEffect.h"

#include "D3DHelpers.h"

namespace IED
{
	using namespace DirectX;

	D3DEffect::D3DEffect(
		ID3D11Device*     a_device,
		D3DVertexShaderID a_vertexShader,
		D3DPixelShaderID  a_pixelShader) noexcept(false) :
		m_vertexShader(a_vertexShader),
		m_pixelShader(a_pixelShader)
	{
		m_constants.fogVector = g_XMZero;
		m_constants.fogColor  = g_XMZero;

		constexpr XMVECTORF32 defaultDirections[3] = {
			{ { { -0.5265408f, -0.5735765f, -0.6275069f, 0 } } },
			{ { { 0.7198464f, 0.3420201f, 0.6040227f, 0 } } },
			{ { { 0.4545195f, -0.7660444f, 0.4545195f, 0 } } },
		};

		constexpr XMVECTORF32 defaultDiffuse[3] = {
			{ { { 1.0000000f, 0.9607844f, 0.8078432f, 0 } } },
			{ { { 0.9647059f, 0.7607844f, 0.4078432f, 0 } } },
			{ { { 0.3231373f, 0.3607844f, 0.3937255f, 0 } } },
		};

		constexpr XMVECTORF32 defaultSpecular[3] = {
			{ { { 1.0000000f, 0.9607844f, 0.8078432f, 0 } } },
			{ { { 0.0000000f, 0.0000000f, 0.0000000f, 0 } } },
			{ { { 0.3231373f, 0.3607844f, 0.3937255f, 0 } } },
		};

		ambientLightColor = { 0.05333332f, 0.09882354f, 0.1819608f, 0 };

		m_constants.specularColorAndPower = { 1, 1, 1, 16 };

		for (int i = 0; i < 3; i++)
		{
			lightDirection[i]     = defaultDirections[i];
			lightDiffuseColor[i]  = defaultDiffuse[i];
			lightSpecularColor[i] = defaultSpecular[i];
		}

		D3D11_BUFFER_DESC desc{};

		desc.ByteWidth      = static_cast<UINT>(sizeof(D3DEffectConstants));
		desc.Usage          = D3D11_USAGE_DEFAULT;
		desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.Usage          = D3D11_USAGE_DYNAMIC;

		ThrowIfFailed(a_device->CreateBuffer(
			std::addressof(desc),
			nullptr,
			m_constantBuffer.ReleaseAndGetAddressOf()));
	}

	void D3DEffect::UpdateWorldMatrix(const NiTransform& a_transform)
	{
		m_world = VectorMath::NiTransformToMatrix4x4(a_transform);
	}

	void D3DEffect::UpdateWorldMatrix(const NiAVObject* a_object)
	{
		m_world = VectorMath::NiTransformToMatrix4x4(a_object->m_worldTransform);
	}

	void XM_CALLCONV D3DEffect::UpdateWorldMatrix(XMMATRIX a_matrix)
	{
		m_world = a_matrix;
	}

	void D3DEffect::SetMatrices(
		const XMMATRIX& a_view,
		const XMMATRIX& a_projection)
	{
		m_view = a_view;
		m_proj = a_projection;
	}

	void D3DEffect::ApplyEffect(
		ID3D11DeviceContext*      a_context,
		const D3DEffectResources& a_resources)
	{
		UpdateConstants();

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		if (SUCCEEDED(a_context->Map(
				m_constantBuffer.Get(),
				0,
				D3D11_MAP_WRITE_DISCARD,
				0,
				std::addressof(mappedResource))))
		{
			*static_cast<D3DEffectConstants*>(mappedResource.pData) = m_constants;

			a_context->Unmap(m_constantBuffer.Get(), 0);
		}

		a_context->VSSetShader(a_resources.GetVertexShader(m_vertexShader).Get(), nullptr, 0);
		a_context->PSSetShader(a_resources.GetPixelShader(m_pixelShader).Get(), nullptr, 0);

		// Set the constant buffer.
		auto buffer = m_constantBuffer.Get();

		a_context->VSSetConstantBuffers(0, 1, std::addressof(buffer));
		a_context->PSSetConstantBuffers(0, 1, std::addressof(buffer));
	}

	void XM_CALLCONV D3DEffect::SetDiffuseColor(XMVECTOR a_color)
	{
		diffuseColor = a_color;
		m_dirtyFlags.set(D3DEffectTestDirtyFlags::kMaterial);
	}

	void XM_CALLCONV D3DEffect::SetEmissiveColor(XMVECTOR a_color)
	{
		emissiveColor = a_color;
		m_dirtyFlags.set(D3DEffectTestDirtyFlags::kMaterial);
	}

	void XM_CALLCONV D3DEffect::SetSpecularColor(DirectX::XMVECTOR a_color)
	{
		m_constants.specularColorAndPower = XMVectorSelect(m_constants.specularColorAndPower, a_color, g_XMSelect1110);
	}

	void D3DEffect::SetSpecularPower(float a_value)
	{
		m_constants.specularColorAndPower = XMVectorSetW(m_constants.specularColorAndPower, a_value);
	}

	void D3DEffect::SetLightingEnabled(bool a_switch)
	{
		lightingEnabled = a_switch;
		m_dirtyFlags.set(D3DEffectTestDirtyFlags::kLight);
	}

	void XM_CALLCONV D3DEffect::SetAmbientLightColor(DirectX::FXMVECTOR a_color)
	{
		ambientLightColor = a_color;
		m_dirtyFlags.set(D3DEffectTestDirtyFlags::kMaterial);
	}

	void D3DEffect::SetAlpha(float a_alpha)
	{
		alpha = a_alpha;
		m_dirtyFlags.set(D3DEffectTestDirtyFlags::kMaterial);
	}

	void D3DEffect::UpdateConstants()
	{
		m_constants.world         = XMMatrixTranspose(m_world);
		m_constants.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(XMMatrixMultiply(m_world, m_view), m_proj));

		auto worldInverse = XMMatrixInverse(nullptr, m_world);

		m_constants.worldInverseTranspose[0] = worldInverse.r[0];
		m_constants.worldInverseTranspose[1] = worldInverse.r[1];
		m_constants.worldInverseTranspose[2] = worldInverse.r[2];

		auto viewInverse = XMMatrixInverse(nullptr, m_view);

		m_constants.eyePosition = viewInverse.r[3];

		if (m_dirtyFlags.consume(D3DEffectTestDirtyFlags::kMaterial))
		{
			auto diffuse     = diffuseColor;
			auto alphaVector = XMVectorReplicate(alpha);

			m_constants.emissiveColor = XMVectorMultiply(XMVectorMultiplyAdd(ambientLightColor, diffuse, emissiveColor), alphaVector);

			// xyz = diffuse * alpha, w = alpha.
			m_constants.diffuseColor = XMVectorSelect(alphaVector, XMVectorMultiply(diffuse, alphaVector), g_XMSelect1110);
		}

		if (m_dirtyFlags.consume(D3DEffectTestDirtyFlags::kLightColor))
		{
			if (lightingEnabled)
			{
				for (int i = 0; i < 3; i++)
				{
					m_constants.lightDiffuseColor[i]  = lightDiffuseColor[i];
					m_constants.lightSpecularColor[i] = lightSpecularColor[i];
				}
			}
			else
			{
				for (int i = 0; i < 3; i++)
				{
					m_constants.lightDiffuseColor[i]  = DirectX::g_XMZero;
					m_constants.lightSpecularColor[i] = DirectX::g_XMZero;
				}
			}
		}

		if (m_dirtyFlags.consume(D3DEffectTestDirtyFlags::kLightDirection))
		{
			if (lightingEnabled)
			{
				for (int i = 0; i < 3; i++)
				{
					m_constants.lightDirection[i] = lightDirection[i];
				}
			}
		}
	}

}