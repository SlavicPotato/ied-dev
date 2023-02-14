#pragma once

namespace IED
{
	class FormHolder
	{
	public:
		static constexpr Game::FormID::held_type FID_LAYDOWN_KEYWORD = 0xC482F;

		static constexpr const auto& GetSingleton() noexcept
		{
			return m_Instance;
		}

		static void Initialize();

		BGSKeyword* layDown{ nullptr };

	private:
		FormHolder() = default;

		static FormHolder m_Instance;
	};
}