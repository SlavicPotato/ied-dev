#include "pch.h"

#include "UISettingsInterface.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UISettingsInterface::UISettingsInterface(
			Controller& a_controller) :
			m_controller(a_controller)
		{
		}

		Data::SettingHolder& UISettingsInterface::GetSettings() const noexcept
		{
			return m_controller.GetConfigStore().settings;
		}

		Data::configStore_t& UISettingsInterface::GetConfigStore() const noexcept
		{
			return m_controller.GetConfigStore().active;
		}

		bool UISettingsInterface::GetEffectControllerEnabled() const noexcept
		{
			return m_controller.EffectControllerEnabled();
		}
	}
}