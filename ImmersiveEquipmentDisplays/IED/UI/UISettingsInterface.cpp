#include "pch.h"

#include "UISettingsInterface.h"

#include "IED/Controller/Controller.h"
#include "IED/ReferenceLightController.h"

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

		bool UISettingsInterface::GetShaderProcessingEnabled() const noexcept
		{
			return m_controller.ShaderProcessingEnabled();
		}

		bool UISettingsInterface::GetPhysicsProcessingEnabled() const noexcept
		{
			return m_controller.PhysicsProcessingEnabled();
		}

		bool UISettingsInterface::GetLightsEnabled() const noexcept
		{
			return ReferenceLightController::GetSingleton().GetEnabled();
		}

		void UISettingsInterface::MarkSettingsDirty()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}
	}
}