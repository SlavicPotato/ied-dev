#pragma once

#include <ext/D3D11Backup.h>

namespace IED
{
	enum class D3DCommonFlags : std::uint32_t
	{
		kNone = 0,

		kAlpha = 1u << 0,
		kCull  = 1u << 1,
		kDepth = 1u << 2,
	};

	DEFINE_ENUM_CLASS_BITWISE(D3DCommonFlags);

	enum class D3DObjectRasterizerState : std::uint32_t
	{
		kNone      = 0,
		kWireframe = 1
	};

	class D3DCommon
	{
	public:
		inline static constexpr auto DEFAULT_FLAGS =
			D3DCommonFlags::kCull;

		D3DCommon(
			ID3D11Device*               a_device,
			ID3D11DeviceContext*        a_context,
			const DXGI_SWAP_CHAIN_DESC& a_desc) noexcept(false);

		void PrepareForDraw();

		[[nodiscard]] inline constexpr auto& GetViewport() noexcept
		{
			return m_viewport;
		}

		[[nodiscard]] inline constexpr auto& GetDepthStencilView() noexcept
		{
			return m_depthStencilView;
		}

		/*[[nodiscard]] inline constexpr auto& GetStates() noexcept
		{
			return m_states;
		}*/

		[[nodiscard]] inline constexpr auto& GetViewMatrix() noexcept
		{
			return m_view;
		}

		[[nodiscard]] inline constexpr auto& GetProjectionMatrix() noexcept
		{
			return m_proj;
		}

		[[nodiscard]] inline constexpr auto& GetDevice() const noexcept
		{
			return m_device;
		}

		[[nodiscard]] inline constexpr auto& GetContext() const noexcept
		{
			return m_context;
		}

		[[nodiscard]] void SetRasterizerState(D3DObjectRasterizerState a_rsstate);
		[[nodiscard]] void SetRenderTargets(bool a_depth);

		[[nodiscard]] inline constexpr void EnableDepth(bool a_switch) noexcept
		{
			m_flags.set(D3DCommonFlags::kDepth, a_switch);
		}

		[[nodiscard]] inline constexpr bool GetDepthEnabled() noexcept
		{
			return m_flags.test(D3DCommonFlags::kDepth);
		}

		[[nodiscard]] inline constexpr void EnableAlpha(bool a_switch) noexcept
		{
			m_flags.set(D3DCommonFlags::kAlpha, a_switch);
		}

		[[nodiscard]] D3D11StateBackupImpl& GetStateBackup(bool a_memzero = true);

	private:
		DirectX::XMMATRIX m_view{ DirectX::SimpleMath::Matrix::Identity };
		DirectX::XMMATRIX m_proj{ DirectX::SimpleMath::Matrix::Identity };

		Microsoft::WRL::ComPtr<ID3D11Device>        m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_rsCullClockwise;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_rsWireframe;

		CD3D11_VIEWPORT m_viewport;

		//std::unique_ptr<DirectX::CommonStates> m_states;

		stl::flag<D3DCommonFlags> m_flags{ DEFAULT_FLAGS };

		D3D11StateBackupImpl m_backup;
	};
}
