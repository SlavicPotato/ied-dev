#pragma once

#include "Render/Events.h"

namespace IED
{
	namespace Drivers
	{
		class Render :
			ILog,
			public ::Events::EventDispatcher<Events::D3D11CreateEventPost>,
			public ::Events::EventDispatcher<Events::IDXGISwapChainPresent>
		{
		public:
			static bool Initialize(bool a_prepHook);

			[[nodiscard]] static constexpr auto& GetSingleton() noexcept
			{
				return m_Instance;
			}

			[[nodiscard]] constexpr auto& GetBufferSize() const noexcept
			{
				return m_bufferSize;
			}

			[[nodiscard]] constexpr auto& GetSwapChainInfo() const noexcept
			{
				return m_swapChainDesc;
			}

			[[nodiscard]] constexpr auto& GetDevice() const noexcept
			{
				return m_device;
			}

			[[nodiscard]] constexpr auto& GetContext() const noexcept
			{
				return m_context;
			}

			template <
				class T,
				class = std::enable_if_t<
					std::is_convertible_v<
						Render,
						::Events::EventDispatcher<T>>>>
			[[nodiscard]] constexpr auto& GetEventDispatcher() noexcept
			{
				return static_cast<::Events::EventDispatcher<T>&>(*this);
			}

			FN_NAMEPROC("Render");

		private:
			Render() = default;

			bool InitializeImpl(bool a_prepHook);

			static void Present_Pre_Hook(std::uint32_t a_p1);
			static void CreateD3D11_Hook();

			void InitializeD3D();

			decltype(&CreateD3D11_Hook) m_createD3D11_o{ nullptr };
			decltype(&Present_Pre_Hook) m_unkPresent_o{ nullptr };

			Microsoft::WRL::ComPtr<ID3D11Device>        m_device;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

			NiPoint3             m_bufferSize;
			DXGI_SWAP_CHAIN_DESC m_swapChainDesc{};

			static inline const auto m_createD3D11_a = IAL::Address<std::uintptr_t>(75595, 77226, 0x9, 0x275);
			static inline const auto m_unkPresent_a  = IAL::Address<std::uintptr_t>(75461, 77246, 0x9, 0x9);

			static Render m_Instance;
		};

	}
}