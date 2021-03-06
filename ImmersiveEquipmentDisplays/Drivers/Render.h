#pragma once

#include "Render/Events.h"

namespace IED
{
	namespace Drivers
	{
		class Render :
			ILog,
			public ::Events::EventDispatcher<Events::D3D11CreateEventPost>,
			public ::Events::EventDispatcher<Events::IDXGISwapChainPresent>,
			public ::Events::EventDispatcher<Events::PrepareGameDataEvent>
		{
		public:
			static bool Initialize(bool a_prepHook);

			[[nodiscard]] inline static constexpr auto& GetSingleton() noexcept
			{
				return m_Instance;
			}

			[[nodiscard]] inline constexpr auto& GetBufferSize() const noexcept
			{
				return m_bufferSize;
			}

			[[nodiscard]] inline constexpr auto& GetSwapChainInfo() const noexcept
			{
				return m_swapChainDesc;
			}
			
			[[nodiscard]] inline constexpr auto& GetDevice() const noexcept
			{
				return m_device;
			}
			
			[[nodiscard]] inline constexpr auto& GetContext() const noexcept
			{
				return m_context;
			}

			template <
				class T,
				class = std::enable_if_t<
					std::is_convertible_v<
						Render,
						::Events::EventDispatcher<T>>>>
			[[nodiscard]] inline constexpr auto& GetEventDispatcher() noexcept
			{
				return static_cast<::Events::EventDispatcher<T>&>(*this);
			}

			FN_NAMEPROC("Render");

		private:
			Render() = default;

			static void Present_Pre_Hook(std::uint32_t a_p1);
			static void PrepareData_Hook(Game::ProcessLists* a_pl, float a_frameTimerSlow);
			static void CreateD3D11_Hook();

			void InitializeD3D();

			decltype(&CreateD3D11_Hook) m_createD3D11_o{ nullptr };
			decltype(&Present_Pre_Hook) m_unkPresent_o{ nullptr };
			decltype(&PrepareData_Hook) m_prepData_o{ nullptr };

			Microsoft::WRL::ComPtr<ID3D11Device>        m_device;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;

			NiPoint3             m_bufferSize;
			DXGI_SWAP_CHAIN_DESC m_swapChainDesc{};

			static inline const auto m_createD3D11_a = IAL::Address<std::uintptr_t>(75595, 77226, 0x9, 0x275);
			static inline const auto m_unkPresent_a  = IAL::Address<std::uintptr_t>(75461, 77246, 0x9, 0x9);
			//static inline const auto m_prepData_a    = IAL::Address<std::uintptr_t>(35556, 0, 0x2DC, 0x0);
			static inline const auto m_prepData_a = IAL::Address<std::uintptr_t>(35565, 36564, 0x53C, 0x8E6);

			static Render m_Instance;
		};

	}
}