#pragma once

#include "UICommon.h"

#include "PopupQueue/UIPopupQueue.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIPopupInterface
		{
		public:
			UIPopupInterface(Controller& a_controller);

			template <class... Args>
			void QueueNotification(
				const char* a_title,
				const char* a_message,
				Args&&... a_v) const;

		private:
			UIPopupQueue& GetPopupQueue() const noexcept;

			Controller& m_controller;
		};

		template <class... Args>
		void UIPopupInterface::QueueNotification(
			const char* a_title,
			const char* a_message,
			Args&&... a_v) const
		{
			auto& queue = GetPopupQueue();
			queue.push(
				UIPopupType::Message,
				a_title,
				a_message,
				std::forward<Args>(a_v)...);
		}
	}
}