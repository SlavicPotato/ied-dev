#pragma once

#include "Drivers/Input/Handlers.h"

namespace IED
{
	namespace KB
	{
		enum class KeyToggleStateEntryFlags : std::uint8_t
		{
			kNone = 0
		};

		DEFINE_ENUM_CLASS_BITWISE(KeyToggleStateEntryFlags);

		struct KeyToggleStateEntry
		{
		public:
			[[nodiscard]] constexpr bool Enabled() const noexcept
			{
				return key != 0;
			}

			[[nodiscard]] constexpr bool GetState() const noexcept
			{
				return state;
			}
			
			constexpr void SetState(bool a_state) noexcept
			{
				state = a_state;
			}

			bool ProcessEvent(const Handlers::KeyEvent& a_evn) noexcept;

			std::uint32_t                       key{ 0 };
			std::uint32_t                       comboKey{ 0 };
			stl::flag<KeyToggleStateEntryFlags> flags{ KeyToggleStateEntryFlags::kNone };

		private:

			bool state{ false };
			bool comboKeyDown{ false };
		};
	}
}