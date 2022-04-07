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
			using queue_type = stl::queue<UIPopupAction>;

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

			inline void clear() noexcept(
				std::is_nothrow_move_assignable_v<queue_type>)
			{
				m_queue = {};
			}

		private:
			queue_type m_queue;
		};

	}
}