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
			if (ImGui::BeginChild("transform_editor_global", { -1.0f, 0.0f }))
			{
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
			m_data = {};
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
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		Data::SettingHolder::EditorPanelCommon& UINodeOverrideEditorGlobal::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor;
		}

		void UINodeOverrideEditorGlobal::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UINodeOverrideEditorGlobal::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			store.settings.Set(
				store.settings.data.ui.transformEditor.globalSex,
				a_newSex);
		}

		void UINodeOverrideEditorGlobal::ApplyProfile(
			profileSelectorParamsNodeOverride_t<int>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			GetOrCreateConfigHolder(a_data.handle) = a_profile.Data();
			UpdateData();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::MergeProfile(
			profileSelectorParamsNodeOverride_t<int>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			auto& conf = GetOrCreateConfigHolder(a_data.handle);

			for (auto& e : a_profile.Data().data)
			{
				conf.data.insert_or_assign(e.first, e.second);
			}

			UpdateData();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		const NodeOverrideProfile::base_type& UINodeOverrideEditorGlobal::GetData(
			const profileSelectorParamsNodeOverride_t<int>& a_params)
		{
			auto& store = m_controller.GetConfigStore();
			return store.active.transforms.GetGlobalData()[0];
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			int a_handle,
			const SingleNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfig(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			int a_handle,
			const SingleNodeOverrideParentUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfig(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		/*void UINodeOverrideEditorGlobal::OnUpdate(
			int a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			GetOrCreateConfigHolder(a_handle) = a_params.data;

			m_controller.RequestEvaluateTransformsAll(true);
		}*/

		void UINodeOverrideEditorGlobal::OnClear(
			int a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetGlobalData()[0];

			if (data.data.erase(a_params.name) > 0)
			{
				m_controller.RequestEvaluateTransformsAll(true);
			}
		}

		void UINodeOverrideEditorGlobal::OnClearParent(
			int a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetGlobalData()[0];

			if (data.placementData.erase(a_params.name) > 0)
			{
				m_controller.RequestEvaluateTransformsAll(true);
			}
		}

		Data::configNodeOverrideHolder_t& UINodeOverrideEditorGlobal::GetOrCreateConfigHolder(int) const
		{
			return m_controller.GetConfigStore().active.transforms.GetGlobalData()[0];
		}

		UIPopupQueue& UINodeOverrideEditorGlobal::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		void UINodeOverrideEditorGlobal::UpdateData()
		{
			m_data = m_controller.GetConfigStore().active.transforms.GetGlobalData()[0];
		}
	}
}