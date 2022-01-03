#include "pch.h"

#include "UINodeOverrideEditorGlobal.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideEditorGlobal::UINodeOverrideEditorGlobal(Controller& a_controller) :
			UINodeOverrideEditorCommon<int>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			UISettingsInterface(a_controller),
			UIGlobalConfigTypeSelectorWidget(a_controller),
			m_controller(a_controller)
		{
		}

		void UINodeOverrideEditorGlobal::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformEditor.globalSex, false);
		}

		void UINodeOverrideEditorGlobal::Draw()
		{
			if (ImGui::BeginChild("no_editor_global", { -1.0f, 0.0f }))
			{
				auto& config = m_controller.GetConfigStore().settings;

				if (DrawTypeSelectorRadio(config.data.ui.transformEditor.globalType))
				{
					config.mark_dirty();
					UpdateData();
				}

				ImGui::Separator();
				ImGui::Spacing();

				DrawNodeOverrideEditor(0, m_data);
			}

			ImGui::EndChild();
		}

		void UINodeOverrideEditorGlobal::OnOpen()
		{
			Reset();
		}

		void UINodeOverrideEditorGlobal::OnClose()
		{
			m_data.clear();
		}

		void UINodeOverrideEditorGlobal::Reset()
		{
			UpdateData();
		}

		constexpr Data::ConfigClass UINodeOverrideEditorGlobal::GetConfigClass() const
		{
			return Data::ConfigClass::Global;
		}

		auto UINodeOverrideEditorGlobal::GetCurrentData()
			-> NodeOverrideEditorCurrentData
		{
			return { 0, std::addressof(m_data) };
		}

		UIData::UICollapsibleStates& UINodeOverrideEditorGlobal::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.transformEditor
			    .colStates[stl::underlying(Data::ConfigClass::Global)];
		}

		void UINodeOverrideEditorGlobal::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		Data::SettingHolder::EditorPanelCommon& UINodeOverrideEditorGlobal::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor;
		}

		void UINodeOverrideEditorGlobal::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UINodeOverrideEditorGlobal::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			store.settings.set(
				store.settings.data.ui.transformEditor.globalSex,
				a_newSex);
		}

		void UINodeOverrideEditorGlobal::ApplyProfile(
			const profileSelectorParamsNodeOverride_t<int>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			GetOrCreateConfigHolder(a_data.handle) = a_profile.Data();
			UpdateData();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::MergeProfile(
			const profileSelectorParamsNodeOverride_t<int>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			MergeProfileData(a_data, a_profile);
			UpdateData();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			int a_handle,
			const SingleNodeOverrideTransformUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			int a_handle,
			const SingleNodeOverridePlacementUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			int a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params);

			auto& store = m_controller.GetConfigStore();
			auto index = stl::underlying(store.settings.data.ui.transformEditor.globalType);

			a_params.data = {
				store.active.transforms.GetGlobalData()[index],
				Data::ConfigClass::Global
			};

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnClearTransform(
			int,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();
			auto index = stl::underlying(store.settings.data.ui.transformEditor.globalType);

			auto& data = store.active.transforms.GetGlobalData()[index];

			if (data.data.erase(a_params.name) > 0)
			{
				m_controller.RequestEvaluateTransformsAll(true);
			}
		}

		void UINodeOverrideEditorGlobal::OnClearPlacement(
			int,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();
			auto index = stl::underlying(store.settings.data.ui.transformEditor.globalType);

			auto& data = store.active.transforms.GetGlobalData()[index];

			if (data.placementData.erase(a_params.name) > 0)
			{
				m_controller.RequestEvaluateTransformsAll(true);
			}
		}

		void UINodeOverrideEditorGlobal::OnClearAllTransforms(
			int,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();
			auto index = stl::underlying(store.settings.data.ui.transformEditor.globalType);

			auto& data = store.active.transforms.GetGlobalData()[index];

			data.data.clear();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnClearAllPlacement(
			int,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();
			auto index = stl::underlying(store.settings.data.ui.transformEditor.globalType);

			auto& data = store.active.transforms.GetGlobalData()[index];

			data.placementData.clear();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		Data::configNodeOverrideHolder_t& UINodeOverrideEditorGlobal::GetOrCreateConfigHolder(int) const
		{
			auto& store = m_controller.GetConfigStore();
			auto index = stl::underlying(store.settings.data.ui.transformEditor.globalType);

			return store.active.transforms.GetGlobalData()[index];
		}

		UIPopupQueue& UINodeOverrideEditorGlobal::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UINodeOverrideEditorGlobal::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		void UINodeOverrideEditorGlobal::UpdateData()
		{
			auto& store = m_controller.GetConfigStore();
			auto index = stl::underlying(store.settings.data.ui.transformEditor.globalType);

			m_data = {
				store.active.transforms.GetGlobalData()[index],
				Data::ConfigClass::Global
			};
		}
	}
}