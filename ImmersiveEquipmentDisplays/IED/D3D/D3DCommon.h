#pragma once

#include "D3DEffectResources.h"

#include <ext/D3D11Backup.h>

#include "Common/VectorMath.h"

namespace IED
{
	enum class D3DCommonFlags : std::uint32_t
	{
		kNone = 0,
	};

	DEFINE_ENUM_CLASS_BITWISE(D3DCommonFlags);

	enum class D3DObjectRasterizerState : std::uint32_t
	{
		kNone      = 0,
		kWireframe = 1
	};

	enum class D3DDepthStencilState : std::uint32_t
	{
		kNone  = 0,
		kWrite = 1
	};

	class D3DCommon :
		public D3DEffectResources
	{
	public:
		static constexpr auto DEFAULT_FLAGS =
			D3DCommonFlags::kNone;

		D3DCommon(
			ID3D11Device*               a_device,
			ID3D11DeviceContext*        a_context,
			const DXGI_SWAP_CHAIN_DESC& a_desc) noexcept(false);

		//void ResetRDV(bool a_renderTarget, bool a_depth);

		void PreDraw();
		void PostDraw();

		[[nodiscard]] constexpr auto& GetViewport() const noexcept
		{
			return m_viewport;
		}

		[[nodiscard]] constexpr auto& GetDepthStencilView() const noexcept
		{
			return m_depthStencilView;
		}

		/*[[nodiscard]] constexpr auto& GetStates() noexcept
		{
			return m_states;
		}*/

		[[nodiscard]] constexpr auto& GetViewMatrix() noexcept
		{
			return m_view;
		}

		[[nodiscard]] constexpr auto& GetViewMatrix() const noexcept
		{
			return m_view;
		}

		[[nodiscard]] constexpr auto& GetProjectionMatrix() noexcept
		{
			return m_proj;
		}

		[[nodiscard]] constexpr auto& GetProjectionMatrix() const noexcept
		{
			return m_proj;
		}

		[[nodiscard]] constexpr auto& GetCameraPosition() noexcept
		{
			return m_camPos;
		}

		[[nodiscard]] constexpr auto& GetCameraPosition() const noexcept
		{
			return m_camPos;
		}

		[[nodiscard]] constexpr auto& GetCameraMatrix() noexcept
		{
			return m_camMatrix;
		}

		[[nodiscard]] constexpr auto& GetCameraMatrix() const noexcept
		{
			return m_camMatrix;
		}
		
		[[nodiscard]] constexpr auto& GetCameraSetMatrix() noexcept
		{
			return m_camSetMatrix;
		}

		[[nodiscard]] constexpr auto& GetCameraSetMatrix() const noexcept
		{
			return m_camSetMatrix;
		}

		[[nodiscard]] constexpr auto& GetDevice() const noexcept
		{
			return m_device;
		}

		[[nodiscard]] constexpr auto& GetContext() const noexcept
		{
			return m_context;
		}

		void SetRasterizerState(D3DObjectRasterizerState a_rsstate);
		void SetRenderTargets(bool a_depth);
		void SetDepthStencilState(D3DDepthStencilState a_state);

		[[nodiscard]] D3D11StateBackupImpl& GetStateBackup(bool a_memzero = true);

	private:
		static void CreateTextureResourceFromView(
			ID3D11Device*                            a_device,
			Microsoft::WRL::ComPtr<ID3D11Texture2D>& a_out,
			ID3D11View*                              a_source) noexcept(false);

		static void CopyResource(
			ID3D11DeviceContext* a_context,
			ID3D11View*          a_dest,
			ID3D11View*          a_source);

		DirectX::XMMATRIX m_view{ VectorMath::g_identity };
		DirectX::XMMATRIX m_proj{ VectorMath::g_identity };

		DirectX::XMMATRIX m_camMatrix{ VectorMath::g_identity };
		DirectX::XMVECTOR m_camPos{ DirectX::g_XMZero.v };

		std::optional<DirectX::XMMATRIX> m_camSetMatrix;

		Microsoft::WRL::ComPtr<ID3D11Device>        m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStateRead;
		Microsoft::WRL::ComPtr<ID3D11BlendState>        m_blendState;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_rsCullClockwise;
		Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_rsWireframe;

		/*Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilViewGameCopy;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetViewGameCopy;*/

		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		CD3D11_VIEWPORT                                m_viewport;

		//std::unique_ptr<DirectX::CommonStates> m_states;

		stl::flag<D3DCommonFlags> m_flags{ DEFAULT_FLAGS };

		D3D11StateBackupImpl m_backup;
	};
}
