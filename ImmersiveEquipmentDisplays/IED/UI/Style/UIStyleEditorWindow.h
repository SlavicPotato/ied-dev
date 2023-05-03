#pragma once

#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/UI/Profile/UIProfileEditorBase.h"

#include "Widgets/UIStyleEditorWidget.h"

#include "Drivers/UI/StyleProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIStyleEditorWindow :
			public UIProfileEditorBase<StyleProfile>,
			public UIStyleEditorWidget
		{
		public:
			static constexpr auto CHILD_ID = ChildWindowID::kStyleEditor;

			UIStyleEditorWindow(Controller& a_controller);
			~UIStyleEditorWindow() noexcept override;

			void Initialize() override;

			std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			void DrawProfileEditorMenuBarItems() override;

			void OnOpen() override;

			void DrawItem(StyleProfile& a_profile) override;

			ProfileManager<StyleProfile>& GetProfileManager() const override;
			UIPopupQueue&                 GetPopupQueue_ProfileBase() const override;

			void OnItemSelected(
				const stl::fixed_string& a_item) override;

			void OnProfileRename(
				const stl::fixed_string& a_oldName,
				const stl::fixed_string& a_newName) override;

			void OnProfileDelete(
				const stl::fixed_string& a_name) override;

			void OnProfileSave(
				const stl::fixed_string& a_name,
				StyleProfile&            a_profile) override;

			void OnProfileReload(
				const StyleProfile& a_profile) override;

			WindowLayoutData GetWindowDimensions() const override;

			void SelectCurrentStyle();
			void DrawActionMenuItems(
				const stl::fixed_string& a_name,
				StyleProfile&            a_data);

			bool m_wantSelectCurrent{ false };

			Controller& m_controller;
		};
	}
}