#include "pch.h"

#include "IUINotification.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	void IUINotification::UIEnableNotifications(bool a_switch)
	{
		if (a_switch)
		{
			gLog.AddSink(this);
		}
		else
		{
			gLog.RemoveSink(this);
		}
	}

	static std::optional<ImVec4> select_color(LogLevel a_level)
	{
		switch (a_level)
		{
		case LogLevel::FatalError:
			return UI::UICommon::g_colorFatalError;
		case LogLevel::Error:
			return UI::UICommon::g_colorError;
		case LogLevel::Warning:
			return UI::UICommon::g_colorWarning;
		case LogLevel::Debug:
			return UI::UICommon::g_colorGreyed;
		default:
			return {};
		}
	}

	void IUINotification::Receive(const LoggerMessageEvent& a_evn)
	{
		if (a_evn.level <= m_llThreshold)
		{
			QueueToastAsync(a_evn.message, select_color(a_evn.level));
		}
	}
}