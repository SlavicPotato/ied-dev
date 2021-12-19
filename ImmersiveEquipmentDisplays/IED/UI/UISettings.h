#pragma once

#include "UICommon.h"

#include "UITips.h"
#include "Widgets/UIControlKeySelectorWidget.h"

#include "IED/UI/Controls/UICollapsibles.h"
#include "IED/UI/Window/UIWindow.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISettings :
			public UIWindow,
			UICollapsibles,
			UIControlKeySelectorWidget,
			public virtual UITipsInterface
		{
			inline static constexpr auto WINDOW_ID = "IED_settings";

		public:
			UISettings(Controller& a_controller);

			void Draw();

		private:
			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void OnCollapsibleStatesUpdate();

			void DrawMenuBar();
			void DrawFileMenu();
			void DrawToolsMenu();
			void DrawMaintenanceMenu();

			void DrawGeneralSection();
			void DrawDisplaysSection();
			void DrawUISection();
			void DrawSoundSection();
			void DrawLogLevelSelector();
			void DrawObjectDatabaseSection();
			void DrawLocalizationSection();

			void DrawFontSelector();
			void DrawExtraGlyphs();

			SetObjectWrapper<float> m_scaleTemp;
			SetObjectWrapper<float> m_fontSizeTemp;

			Controller& m_controller;
		};

	}  // namespace UI
}  // namespace IED