#pragma once

#include "Events/Dispatcher.h"
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
			static bool Initialize();

			[[nodiscard]] inline static constexpr auto& GetSingleton() noexcept
			{
				return m_Instance;
			}

			[[nodiscard]] inline constexpr const auto& GetBufferSize() const noexcept
			{
				return m_bufferSize;
			}

			[[nodiscard]] inline constexpr auto GetDevice() const noexcept
			{
				return m_device;
			}

			[[nodiscard]] inline constexpr auto GetDeviceContext() const noexcept
			{
				return m_context;
			}

			[[nodiscard]] inline constexpr auto GetSwapChain() const noexcept
			{
				return m_swapChain;
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

			static void Present_Pre(std::uint32_t a_p1);
			static void CreateD3D11_Hook();

			decltype(&CreateD3D11_Hook) m_createD3D11_o{ nullptr };
			decltype(&Present_Pre) m_unkPresent_o{ nullptr };

			ID3D11Device* m_device{ nullptr };
			ID3D11DeviceContext* m_context{ nullptr };
			IDXGISwapChain* m_swapChain{ nullptr };
			NiPoint3 m_bufferSize;

			static inline auto m_createD3D11_a = IAL::Addr(75595, 77226, 0x9, 0x275);
			static inline auto m_unkPresent_a = IAL::Addr(75461, 77246, 0x9, 0x9);

			static Render m_Instance;
		};

	}  // namespace Drivers
}  // namespace IED