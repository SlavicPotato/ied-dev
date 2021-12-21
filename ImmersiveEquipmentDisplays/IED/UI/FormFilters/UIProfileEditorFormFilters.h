#pragma once

#include "../UICommon.h"

#include "../Widgets/Form/UIFormFilterWidget.h"
#include "../Widgets/Form/UIFormSelectorWidget.h"

#include "IED/GlobalProfileManager.h"

#include "IED/UI/Profile/UIProfileEditorBase.h"

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
			UIProfileEditorFormFilters(Controller& a_controller);

			void Initialize();

			virtual void DrawProfileEditorMenuBarItems() override;

			inline bool ChangedConfig() const
			{
				bool res = m_changedConfig;
				if (res)
				{
					m_changedConfig = false;
				}
				return res;
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
				FormFilterProfile& a_profile) override;

			virtual void OnProfileReload(
				const FormFilterProfile& a_profile) override;

			virtual void OnCollapsibleStatesUpdate();

			virtual WindowLayoutData GetWindowDimensions() const;

			void DrawMenuBarItems();

			mutable bool m_changedConfig{ false };

			UIFormSelectorWidget m_formSelector;

			Controller& m_controller;
		};
	}  // namespace UI
}  // namespace IED