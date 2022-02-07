#pragma once

#include "KeyEventType.h"

namespace IED
{
	class KeyState
	{
	public:
		void UpdateKeyState(UINT a_key);
		void ResetKeyState();

		void SetKeyState(KeyEventType a_ev, std::uint32_t a_sc, UINT a_vk);
		void SetKeyState(KeyEventType a_ev, UINT a_key);
		void ClearKeyState(UINT a_key);

		[[nodiscard]] inline constexpr const auto& GetKeyStateData() const noexcept
		{
			return m_data;
		}

		inline static constexpr BYTE KS_MS_BIT = 1ui8 << 7;
		inline static constexpr BYTE KS_LS_BIT = 1ui8 << 0;

	private:
		void SetLRKeyState(KeyEventType a_ev, std::uint32_t a_sc, UINT a_vk);

		using state_data_type = BYTE[256];

		state_data_type m_data{ 0 };
	};

}