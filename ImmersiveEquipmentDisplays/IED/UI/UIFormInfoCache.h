#pragma once

#include "IED/Controller/IForm.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIFormInfoCache
		{
			struct entry_t
			{
				std::uint64_t fcaccess{ 0 };
				std::unique_ptr<formInfoResult_t> info;
			};

			inline static constexpr std::size_t CLEANUP_THRESHOLD = 1250;
			inline static constexpr std::size_t CLEANUP_TARGET = 1000;
			inline static constexpr long long CLEANUP_RUN_INTERVAL = 30000000;

		public:
			UIFormInfoCache(
				Controller& a_controller);

			inline void clear() noexcept
			{
				m_data.swap(decltype(m_data)());
			}

			inline auto size() const noexcept
			{
				return m_data.size();
			}

			const formInfoResult_t* LookupForm(Game::FormID a_form);

			void RunCleanup();

		private:
			long long m_lastCleanup{ 0 };

			std::unordered_map<Game::FormID, entry_t> m_data;

			Controller& m_controller;
		};

	}
}