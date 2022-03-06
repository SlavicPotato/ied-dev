#pragma once

#include "UIPopupAction.h"

#include "IED/UI/Modals/UICommonModals.h"

namespace IED
{
	namespace UI
	{
		class UIPopupQueue :
			UICommonModals
		{
		public:
			UIPopupQueue(Localization::ILocalization& a_localization);

			UIPopupQueue(const UIPopupQueue&) = delete;
			UIPopupQueue(UIPopupQueue&&)      = delete;

			UIPopupQueue& operator=(const UIPopupQueue&) = delete;
			UIPopupQueue& operator=(UIPopupQueue&&) = delete;

			void run();

			template <class... Args>
			decltype(auto) push(Args&&... a_v)
			{
				return m_queue.emplace(std::forward<Args>(a_v)...);
			}

			inline void clear() noexcept
			{
				m_queue = {};
			}

		private:
			std::queue<UIPopupAction> m_queue;
		};

	}
}