#pragma once

#include "KeyEventType.h"

namespace IED
{
	class KeyState
	{
	public:
		void UpdateKeyState(UINT a_key);
		void ResetKeyState();

		void SetKeyState(KeyEventState a_ev, std::uint32_t a_sc, UINT a_vk);
		void SetKeyState(KeyEventState a_ev, UINT a_key);
		void ClearKeyState(UINT a_key);

		[[nodiscard]] constexpr const auto& GetKeyStateData() const noexcept
		{
			return m_data;
		}

		static constexpr BYTE KS_MS_BIT = 1ui8 << 7;
		static constexpr BYTE KS_LS_BIT = 1ui8 << 0;

	private:
		void SetLRKeyState(KeyEventState a_ev, std::uint32_t a_sc, UINT a_vk);

		using state_data_type = BYTE[256];

		state_data_type m_data{ 0 };
	};

}