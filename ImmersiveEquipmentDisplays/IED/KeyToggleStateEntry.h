#pragma once

#include "Drivers/Input/Handlers.h"

#include "IED/ConfigKeybind.h"

namespace IED
{
	namespace KB
	{
		struct KeyToggleStateEntry
		{
		public:
			KeyToggleStateEntry() = default;

			KeyToggleStateEntry(
				const Data::configKeybindEntry_t& a_entry) :
				key(a_entry.key),
				comboKey(a_entry.comboKey),
				numStates(std::max(a_entry.numStates, 1u))
			{
			}

			KeyToggleStateEntry& operator=(
				const Data::configKeybindEntry_t& a_entry) noexcept
			{
				key      = a_entry.key;
				comboKey = a_entry.comboKey;
				SetNumStates(a_entry.numStates);

				return *this;
			}

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

		private:
			std::uint32_t state{ 0 };
			bool          comboKeyDown{ false };
		};
	}
}