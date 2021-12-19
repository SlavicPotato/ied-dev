#pragma once

namespace IED
{
	class FormHolder
	{
	public:

		static inline constexpr Game::FormID::held_type FID_LAYDOWN_KEYWORD = 0xC482F; 

		static inline constexpr const auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		static void Populate();

		BGSKeyword* layDown{ nullptr };

	private:
		FormHolder() = default;

		static FormHolder m_Instance;
	};
}