#pragma once

#include "IUI.h"

namespace IED
{
	class IUINotification :
		public IUI,
		::Events::EventSink<LoggerMessageEvent>
	{
	public:
		IUINotification()                            = default;
		virtual ~IUINotification() noexcept override = default;

		void UIEnableNotifications(bool a_switch);

		inline constexpr void UISetLogNotificationThreshold(LogLevel a_level) noexcept
		{
			m_llThreshold = a_level;
		}

		[[nodiscard]] inline constexpr auto UIGetLogNotificationThreshold() const noexcept
		{
			return m_llThreshold;
		}

	private:
		virtual void Receive(const LoggerMessageEvent& a_evn) override;

		LogLevel m_llThreshold{ LogLevel::Message };
	};
}