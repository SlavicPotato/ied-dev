#pragma once

#include "UIEditorInterface.h"

#include "IED/ConfigCommon.h"
#include "IED/SettingHolder.h"

#include "UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIEditorTabPanel :
			public virtual UILocalizationInterface
		{
			struct Interface
			{
				UIEditorInterface* ptr{ nullptr };
				Localization::StringID label;

				ImGuiTabBarFlags flags{ ImGuiTabItemFlags_None };
			};

		public:
			UIEditorTabPanel(
				Controller& a_controller,
				Localization::StringID a_menuName);

			void Initialize();
			void Reset();
			void QueueUpdateCurrent();
			void Draw();
			void DrawMenuBarItems();
			void OnOpen();
			void OnClose();

			void SetEditor(
				Data::ConfigClass a_class,
				UIEditorInterface& a_interface,
				Localization::StringID a_label);

		private:
			virtual Data::SettingHolder::EditorPanel& GetEditorConfig() = 0;

			void SetTabSelected(
				Data::ConfigClass a_class);

			void EvaluateTabSwitch(
				Data::ConfigClass a_class);

			Interface m_interfaces[Data::CONFIG_CLASS_MAX];

			Data::ConfigClass m_currentClass{ Data::ConfigClass::Global };

			Localization::StringID m_menuName;

			Controller& m_controller;
		};

	}  // namespace UI
}  // namespace IED