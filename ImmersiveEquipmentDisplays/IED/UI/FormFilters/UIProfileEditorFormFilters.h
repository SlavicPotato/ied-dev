#pragma once

#include "IED/UI/Widgets/Form/UIFormFilterWidget.h"
#include "IED/UI/Widgets/Form/UIFormSelectorWidget.h"

#include "IED/UI/Profile/UIProfileEditorBase.h"

#include "IED/UI/UICommon.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	struct FFPEFormFilterParams_t
	{
		FormFilterProfile& profile;
	};

	namespace UI
	{
		class UIProfileEditorFormFilters :
			public UIProfileEditorBase<FormFilterProfile>,
			UIFormFilterWidget<FFPEFormFilterParams_t>
		{
		public:
			static constexpr auto CHILD_ID = ChildWindowID::kUIProfileEditorFormFilters;

			UIProfileEditorFormFilters(Controller& a_controller);
			~UIProfileEditorFormFilters() noexcept override;

			void Initialize();

			virtual void DrawProfileEditorMenuBarItems() override;

			[[nodiscard]] constexpr bool ChangedConfig() const
			{
				const bool res = m_changedConfig;
				if (res)
				{
					m_changedConfig = false;
				}
				return res;
			}

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

		private:
			virtual void DrawItem(FormFilterProfile& a_profile) override;

			virtual UIPopupQueue& GetPopupQueue_ProfileBase() const override;

			virtual ProfileManager<FormFilterProfile>& GetProfileManager() const override;

			virtual void OnItemSelected(
				const stl::fixed_string& a_item) override;

			virtual void OnProfileRename(
				const stl::fixed_string& a_oldName,
				const stl::fixed_string& a_newName) override;

			virtual void OnProfileDelete(
				const stl::fixed_string& a_name) override;

			virtual void OnProfileSave(
				const stl::fixed_string& a_name,
				FormFilterProfile&       a_profile) override;

			virtual void OnProfileReload(
				const FormFilterProfile& a_profile) override;

			virtual void OnCollapsibleStatesUpdate();

			virtual WindowLayoutData GetWindowDimensions() const;

			void DrawMenuBarItems();

			mutable bool m_changedConfig{ false };

			UIFormSelectorWidget m_formSelector;

			Controller& m_controller;
		};
	}
}