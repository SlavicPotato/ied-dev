#pragma once

//#define IED_D3D_MK_SHADER_BYTECODE

namespace IED
{
	enum class D3DVertexShaderID
	{
		kLightingColorVertexShader,
		kColorVertexShader,

		kMax
	};

	enum class D3DPixelShaderID
	{
		kLightingPixelShader,
		kBasicPixelShader,

		kMax
	};

	class D3DEffectResources
	{
	public:
		inline static constexpr auto DATA_PATH = "Data\\SKSE\\Plugins\\IED\\Assets\\Shaders\\blob.bin";

		D3DEffectResources(ID3D11Device* a_device) noexcept(false);

		[[nodiscard]] inline constexpr auto& GetVertexShader(D3DVertexShaderID a_id) const noexcept
		{
			return m_vertexShaders[stl::underlying(a_id)];
		}

		[[nodiscard]] inline constexpr auto& GetPixelShader(D3DPixelShaderID a_id) const noexcept
		{
			return m_pixelShaders[stl::underlying(a_id)];
		}

		[[nodiscard]] inline constexpr auto& GetILVertexPositionNormalColor() const noexcept
		{
			return m_iaVertexPositionNormalColor;
		}

		[[nodiscard]] inline constexpr auto& GetILVertexPositionColorAV() const noexcept
		{
			return m_iaVertexPositionColorAV;
		}

	private:
#if defined(IED_D3D_MK_SHADER_BYTECODE)
		static void MkShaderBytecode() noexcept(false);
#endif

		[[nodiscard]] inline constexpr auto& GetVertexShader(D3DVertexShaderID a_id) noexcept
		{
			return m_vertexShaders[stl::underlying(a_id)];
		}

		[[nodiscard]] inline constexpr auto& GetPixelShader(D3DPixelShaderID a_id) noexcept
		{
			return m_pixelShaders[stl::underlying(a_id)];
		}

		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShaders[stl::underlying(D3DVertexShaderID::kMax)];
		Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_pixelShaders[stl::underlying(D3DPixelShaderID::kMax)];

		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_iaVertexPositionNormalColor;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_iaVertexPositionColorAV;
	};
}
