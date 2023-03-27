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

			[[nodiscard]] constexpr std::uint32_t GetState() const noexcept
			{
				return state;
			}

			constexpr void SetState(std::uint32_t a_state) noexcept
			{
				state = a_state > numStates ? 0 : a_state;
			}

			constexpr void SetNumStates(std::uint32_t a_numStates) noexcept
			{
				numStates = std::max(a_numStates, 1u);
				state     = state > numStates ? 0 : state;
			}

			bool ProcessEvent(const Handlers::KeyEvent& a_evn) noexcept;

			std::uint32_t                       key{ 0 };
			std::uint32_t                       comboKey{ 0 };
			std::uint32_t                       numStates{ 1 };
			stl::flag<KeyToggleStateEntryFlags> flags{ KeyToggleStateEntryFlags::kNone };

		private:
			std::uint32_t state{ 0 };
			bool          comboKeyDown{ false };
		};
	}
}