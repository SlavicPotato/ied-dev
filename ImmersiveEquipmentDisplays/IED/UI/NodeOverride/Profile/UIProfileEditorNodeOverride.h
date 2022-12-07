#pragma once

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorWidget.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

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
			public UINodeOverrideEditorWidget<UIGlobalEditorDummyHandle>
		{
		public:
			inline static constexpr auto CHILD_ID = ChildWindowID::kUIProfileEditorNodeOverride;

			UIProfileEditorNodeOverride(Controller& a_controller);

			virtual void Initialize() override;

			virtual void DrawProfileEditorMenuBarItems() override;

			virtual std::uint32_t GetContextID() override
			{
				return static_cast<std::uint32_t>(CHILD_ID);
			}

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
				UIGlobalEditorDummyHandle                      a_handle,
				const SingleNodeOverrideTransformUpdateParams& a_params) override;

			virtual void OnUpdate(
				UIGlobalEditorDummyHandle                      a_handle,
				const SingleNodeOverridePlacementUpdateParams& a_params) override;

			virtual void OnUpdate(
				UIGlobalEditorDummyHandle                    a_handle,
				const SingleNodeOverridePhysicsUpdateParams& a_params) override;

			virtual void OnUpdate(
				UIGlobalEditorDummyHandle       a_handle,
				const NodeOverrideUpdateParams& a_params) override;

			virtual void OnClearTransform(
				UIGlobalEditorDummyHandle            a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearPlacement(
				UIGlobalEditorDummyHandle            a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearPhysics(
				UIGlobalEditorDummyHandle            a_handle,
				const ClearNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllTransforms(
				UIGlobalEditorDummyHandle               a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllPlacement(
				UIGlobalEditorDummyHandle               a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual void OnClearAllPhysics(
				UIGlobalEditorDummyHandle               a_handle,
				const ClearAllNodeOverrideUpdateParams& a_params) override;

			virtual WindowLayoutData GetWindowDimensions() const;

			virtual UIPopupQueue& GetPopupQueue() override;

			template <class Tp>
			void UpdateConfigSingle(
				const Tp& a_params,
				bool      a_syncSex);

			void UpdateConfig(const NodeOverrideUpdateParams& a_params);

			NodeOverrideProfile* GetCurrentProfile() const;

			std::optional<cachedItem_t<entryNodeOverrideData_t>> m_cachedItem;

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