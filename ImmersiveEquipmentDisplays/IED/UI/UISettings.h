#pragma once

#include "UICommon.h"

#include "UITips.h"

#include "Widgets/Form/UIFormPickerWidget.h"
#include "Widgets/UIControlKeySelectorWidget.h"
#include "Widgets/UIFormTypeSelectorWidget.h"
#include "Widgets/UIPopupToggleButtonWidget.h"
#include "Widgets/UIStylePresetSelectorWidget.h"

#include "Controls/UICollapsibles.h"

#include "Window/UIWindow.h"
#include "Window/UIWindowBase.h"

#include "IED/ConfigCommon.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UISettings :
			public UIWindow,
			public UIChildWindowBase,
			UICollapsibles,
			UIControlKeySelectorWidget,
			UIFormPickerWidget,
			UIStylePresetSelectorWidget,
			public virtual UIFormTypeSelectorWidget,
			public virtual UITipsInterface
		{
			inline static constexpr auto WINDOW_ID = "ied_settings";

			enum class ContextMenuAction
			{
				None,
				Delete,
				Add
			};

		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUISettings;

			UISettings(Controller& a_controller);

			void Draw() override;

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
			void              DrawEffectShadersSection();
			void              DrawUISection();
			ContextMenuAction DrawSoundContextMenu(Data::ConfigSound<Game::FormID>& a_data);
			void              DrawSoundSection();
			void              DrawLogLevelSelector();
			void              DrawObjectDatabaseSection();
			void              DrawLocalizationSection();

			void DrawFontSelector();
			void DrawExtraGlyphs();
			void DrawFontMiscOptions();

			bool DrawSoundPairs();

			ContextMenuAction DrawSoundPairContextMenu();

			bool DrawSoundPair(
				std::uint8_t                                  a_formType,
				Data::ConfigSound<Game::FormID>::soundPair_t& a_soundPair);

			template <
				class Tf,
				class Tl>
			void DrawCommonResetContextMenu(
				const char* a_imid,
				Tl          a_strid,
				bool        a_enabled,
				Tf          a_func);

			stl::optional<float> m_scaleTemp;
			stl::optional<float> m_fontSizeTemp;

			stl::optional<std::uint8_t> m_tmpFormType;

			Controller& m_controller;
		};

		template <class Tf, class Tl>
		void UISettings::DrawCommonResetContextMenu(
			const char* a_imid,
			Tl          a_strid,
			bool        a_enabled,
			Tf          a_func)
		{
			ImGui::PushID(a_imid);

			//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			//ImGui::PopStyleVar();

			ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(
						LS(a_strid, "1"),
						nullptr,
						false,
						a_enabled))
				{
					a_func();
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

	}
}