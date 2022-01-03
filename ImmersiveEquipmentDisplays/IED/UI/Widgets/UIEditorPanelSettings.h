#pragma once

#include "IED/ConfigOverride.h"

#include "IED/SettingHolder.h"

#include "../UITips.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEditorPanelSettings :
			virtual public UITipsInterface
		{
		public:
			UIEditorPanelSettings(Controller& a_controller);

			void DrawEditorPanelSettings();

			inline constexpr auto GetSex() const noexcept
			{
				return m_sex;
			}

		protected:
			void SetSex(Data::ConfigSex a_sex, bool a_sendEvent);

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() = 0;

			virtual void OnEditorPanelSettingsChange() = 0;

		private:
			virtual void DrawExtraEditorPanelSettings();

			virtual void OnSexChanged(Data::ConfigSex a_newSex) = 0;

			Data::ConfigSex m_sex{ Data::ConfigSex::Male };
		};
	}  // namespace UI
}  // namespace IED