#pragma once

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorWidget.h"

#include "IED/UI/Profile/UIProfileEditorBase.h"

#include "IED/UI/UICommon.h"

#include "IED/GlobalProfileManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UIProfileEditorNodeOverride :
			public UIProfileEditorBase<NodeOverrideProfile>,
			public UINodeOverrideEditorWidget<int>
		{
			struct cachedItem_t
			{
				cachedItem_t() = default;

				template <class... Args>
				cachedItem_t(
					const stl::fixed_string& a_name,
					Args&&... a_args) :
					name(a_name),
					data(std::forward<Args>(a_args)...)
				{
				}

				stl::fixed_string       name;
				entryNodeOverrideData_t data;
			};

		public:
			UIProfileEditorNodeOverride(Controller& a_controller);

			virtual void Initialize() override;

			virtual void DrawProfileEditorMenuBarItems() override;

		private:
			virtual void DrawItem(NodeOverrideProfile& a_profile) override;

			virtual constexpr Data::ConfigClass GetConfigClass() const override;

			virtual ProfileManager<NodeOverrideProfile>& GetProfileManager() const override;
			virtual UIPopupQueue&                        GetPopupQueue_ProfileBase() const override;

			virtual NodeOverrideEditorCurrentData GetCurrentData() override;

			virtual void OnItemSelected(
				const stl::fixed_string& a_item) override;

			virtual void OnProfileRename(
				const stl::fixed_string& a_oldName,
				const stl::fixed_string& a_newName) override;

			virtual void OnProfileDelete(
				const stl::fixed_string& a_name) override;

			virtual void OnProfileSave(
				const stl::fixed_string& a_name,
				NodeOverrideProfile&     a_profile) override;

			virtual void OnProfileReload(
				const NodeOverrideProfile& a_profile) override;

			virtual void OnSexChanged(
				Data::ConfigSex a_newSex) override;

			virtual Data::SettingHolder::EditorPanelCommon& GetEditorPanelSettings() override;

			virtual void OnEditorPanelSettingsChange() override;

			virtual UIData::UICollapsibleStates& GetCollapsibleStatesData();

			virtual void OnCollapsibleStatesUpdate();

			virtual void OnUpdate(
				int                                            a_handle,
				const SingleNodeOverrideTransformUpdateParams& a_params) override;

			virtual void OnUpdate(
				int                                            a_handle,
				const SingleNodeOverridePlacementUpdateParams& a_params) override;

			virtual void OnUpdate(
				int                             a_handle,
				const NodeOverrideUpdateParams& a_params) override;

			virtual void OnClearTransform(
				int                                  a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearPlacement(
				int                                  a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllTransforms(
				int                                     a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllPlacement(
				int                                     a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual WindowLayoutData GetWindowDimensions() const;

			virtual UIPopupQueue& GetPopupQueue() override;

			template <class Tp>
			void UpdateConfigSingle(
				const Tp& a_params,
				bool      a_syncSex);

			void UpdateConfig(const NodeOverrideUpdateParams& a_params);

			NodeOverrideProfile* GetCurrentProfile() const;

			stl::optional<cachedItem_t> m_cachedItem;

			Controller& m_controller;
		};

		template <class Tp>
		void UIProfileEditorNodeOverride::UpdateConfigSingle(
			const Tp& a_params,
			bool      a_syncSex)
		{
			auto profile = GetCurrentProfile();
			if (!profile)
			{
				return;
			}

			auto& confEntry = profile->Data()
			                      .get_data<decltype(a_params.entry.second)>()
			                      .try_emplace(a_params.name)
			                      .first->second;

			if (a_syncSex)
			{
				auto og = Data::GetOppositeSex(a_params.sex);

				a_params.entry.second(og) = a_params.entry.second(a_params.sex);
				confEntry                 = a_params.entry.second;
			}
			else
			{
				confEntry(a_params.sex) = a_params.entry.second(a_params.sex);
			}

			a_params.entry.first = GetConfigClass();
		}

	}
}