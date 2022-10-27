#include "pch.h"

#include "D3DEffectResources.h"

#include "D3DAssets.h"
#include "D3DHelpers.h"
#include "D3DVertices.h"

#include <ext/ISerializationBase.h>

#if defined(IED_D3D_MK_SHADER_BYTECODE)
#	include <d3dcompiler.h>
#endif

namespace IED
{
	D3DEffectResources::D3DEffectResources(
		ID3D11Device* a_device) noexcept(false)
	{
#if defined(IED_D3D_MK_SHADER_BYTECODE)
		MkShaderBytecode();
#endif

		D3DShaderDataHolder holder;
		D3DShaderDataIO::Load(DATA_PATH, holder);

		ThrowIfFailed(a_device->CreateVertexShader(
			holder.vertexColorLighting.shaderData.data(),
			holder.vertexColorLighting.shaderData.size(),
			nullptr,
			GetVertexShader(D3DVertexShaderID::kLightingColorVertexShader).ReleaseAndGetAddressOf()));

		ThrowIfFailed(a_device->CreateVertexShader(
			holder.vertexColor.shaderData.data(),
			holder.vertexColor.shaderData.size(),
			nullptr,
			GetVertexShader(D3DVertexShaderID::kColorVertexShader).ReleaseAndGetAddressOf()));

		ThrowIfFailed(a_device->CreatePixelShader(
			holder.pixelVertexLighting.shaderData.data(),
			holder.pixelVertexLighting.shaderData.size(),
			nullptr,
			GetPixelShader(D3DPixelShaderID::kLightingPixelShader).ReleaseAndGetAddressOf()));

		ThrowIfFailed(a_device->CreatePixelShader(
			holder.pixelBasic.shaderData.data(),
			holder.pixelBasic.shaderData.size(),
			nullptr,
			GetPixelShader(D3DPixelShaderID::kBasicPixelShader).ReleaseAndGetAddressOf()));

		ThrowIfFailed(a_device->CreateInputLayout(
			DirectX::VertexPositionNormalColor::InputElements,
			DirectX::VertexPositionNormalColor::InputElementCount,
			holder.vertexColorLighting.shaderData.data(),
			holder.vertexColorLighting.shaderData.size(),
			m_iaVertexPositionNormalColor.ReleaseAndGetAddressOf()));

		ThrowIfFailed(a_device->CreateInputLayout(
			VertexPositionColorAV::InputElements,
			VertexPositionColorAV::InputElementCount,
			holder.vertexColor.shaderData.data(),
			holder.vertexColor.shaderData.size(),
			m_iaVertexPositionColorAV.ReleaseAndGetAddressOf()));

	}

#if defined(IED_D3D_MK_SHADER_BYTECODE)
	void D3DEffectResources::MkShaderBytecode() noexcept(false)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

		Microsoft::WRL::ComPtr<ID3DBlob> blobVS;
		Microsoft::WRL::ComPtr<ID3DBlob> blobVS2;
		Microsoft::WRL::ComPtr<ID3DBlob> blobPS;
		Microsoft::WRL::ComPtr<ID3DBlob> blobPS2;

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\Effect.fx",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"VSBasicVertexLightingVc",
			"vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blobVS.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf()));

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\Effect.fx",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"PSBasicVertexLighting",
			"ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blobPS.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf()));

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\Effect.fx",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"VSBasicVcNoFog",
			"vs_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blobVS2.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf()));

		ThrowIfFailed(D3DCompileFromFile(
			L"Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\Effect.fx",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"PSBasicNoFog",
			"ps_5_0",
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			blobPS2.ReleaseAndGetAddressOf(),
			errorBlob.ReleaseAndGetAddressOf()));

		ShaderDataHolder h;

		auto bv = static_cast<std::uint8_t*>(blobVS->GetBufferPointer());
		auto bp = static_cast<std::uint8_t*>(blobPS->GetBufferPointer());
		auto be = static_cast<std::uint8_t*>(blobVS2->GetBufferPointer());
		auto bf = static_cast<std::uint8_t*>(blobPS2->GetBufferPointer());

		h.vertexColorLighting.shaderData.assign(bv, bv + blobVS->GetBufferSize());
		h.pixelVertexLighting.shaderData.assign(bp, bp + blobPS->GetBufferSize());
		h.vertexColor.shaderData.assign(be, be + blobVS2->GetBufferSize());
		h.pixelBasic.shaderData.assign(bf, bf + blobPS2->GetBufferSize());

		D3DShaderDataIO::Save(DATA_PATH, h);
	}
#endif
}