#pragma once

#include "UICommon.h"

#include "UITips.h"

#include "Widgets/Form/UIFormPickerWidget.h"
#include "Widgets/UIControlKeySelectorWidget.h"
#include "Widgets/UIFormTypeSelectorWidget.h"
#include "Widgets/UIPopupToggleButtonWidget.h"
#include "Widgets/UIStylePresetSelectorWidget.h"

#include "Controls/UICollapsibles.h"

#include "UIContext.h"
#include "Window/UIWindow.h"

#include "IED/ConfigCommon.h"

namespace IED
{
	class Controller;

	namespace Tasks
	{
		class UIRenderTaskBase;
	};

	namespace UI
	{
		class UISettings :
			public UIContext,
			public UIWindow,
			UICollapsibles,
			UIFormPickerWidget,
			public virtual UIFormTypeSelectorWidget
		{
			static constexpr auto WINDOW_ID = "ied_settings";

			enum class ContextMenuAction
			{
				None,
				Delete,
				Add
			};

		public:
			static constexpr auto CHILD_ID = ChildWindowID::kUISettings;

			UISettings(
				Tasks::UIRenderTaskBase& a_owner,
				Controller&              a_controller);

			void Draw() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();
			virtual void                         OnCollapsibleStatesUpdate();

			void DrawMenuBar();
			void DrawFileMenu();
			void DrawToolsMenu();
			void DrawMaintenanceMenu();

			void              DrawGeneralSection();
			void              DrawDisplaysSection();
			void              DrawGearPosSection();
			void              DrawEffectsSection();
			void              DrawUISection();
			ContextMenuAction DrawSoundContextMenu(Data::ConfigSound<Game::FormID>& a_data);
			void              DrawSoundSection();
			bool              DrawLogLevelSelector(const char *a_id, Localization::StringID a_title, LogLevel& a_value);
			void              DrawObjectDatabaseSection();
			void              DrawLocalizationSection();
			void              DrawI3DISection();

			void DrawFontSelector();
			void DrawExtraGlyphs();
			void DrawFontMiscOptions();

			bool DrawSoundPairs();

			ContextMenuAction DrawSoundPairContextMenu();

			bool DrawSoundPair(
				std::uint8_t                                  a_formType,
				Data::ConfigSound<Game::FormID>::soundPair_t& a_soundPair);

			void DrawCommonResetContextMenu(
				const char*            a_imid,
				Localization::StringID a_strid,
				bool                   a_enabled,
				std::function<void()>  a_func);

			stl::optional<float> m_scaleTemp;
			stl::optional<float> m_fontSizeTemp;

			std::optional<std::uint8_t> m_tmpFormType;

			Tasks::UIRenderTaskBase& m_owner;
			Controller&              m_controller;
		};

	}
}