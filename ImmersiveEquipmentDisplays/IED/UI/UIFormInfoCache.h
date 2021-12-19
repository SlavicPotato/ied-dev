#pragma once

#include "IED/Controller/IForm.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIFormInfoCache
		{
			/*enum class OperationStatus
			{
				Pending,
				Success,
				Failed
			};*/

			struct entry_t
			{
				//OperationStatus status{ OperationStatus::Pending };
				std::unique_ptr<formInfoResult_t> info;
			};

		public:
			UIFormInfoCache(
				Controller& a_controller);

			inline void clear() noexcept
			{
				m_data.swap(decltype(m_data)());
			}

			const formInfoResult_t* LookupForm(Game::FormID a_form);

			void Trim();

		private:

			std::unordered_map<Game::FormID, entry_t> m_data;

			Controller& m_controller;
		};

	}
}