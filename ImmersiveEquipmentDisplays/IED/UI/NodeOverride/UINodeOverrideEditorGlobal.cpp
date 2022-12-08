#include "pch.h"

#include "UINodeOverrideEditorGlobal.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideEditorGlobal::UINodeOverrideEditorGlobal(Controller& a_controller) :
			UINodeOverrideEditorCommon<UIGlobalEditorDummyHandle>(a_controller),
			UISettingsInterface(a_controller),
			m_controller(a_controller)
		{
		}

		UINodeOverrideEditorGlobal::~UINodeOverrideEditorGlobal() noexcept
		{
			GetProfileManager().RemoveSink(this);
		}

		void UINodeOverrideEditorGlobal::EditorInitialize()
		{
			InitializeProfileBase();

			const auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformEditor.globalSex, false);
		}

		void UINodeOverrideEditorGlobal::EditorDraw()
		{
			if (ImGui::BeginChild("no_editor_global", { -1.0f, 0.0f }))
			{
				auto& settings = m_controller.GetConfigStore().settings;

				if (DrawTypeSelectorRadio(settings.data.ui.transformEditor.globalType))
				{
					settings.mark_dirty();
					UpdateData(m_data);
				}

				ImGui::Separator();
				ImGui::Spacing();

				if (settings.data.ui.transformEditor.globalType == Data::GlobalConfigType::Player)
				{
					if (!m_controller.SkeletonCheck(Data::IData::GetPlayerRefID()))
					{
						ImGui::TextColored(
							UICommon::g_colorWarning,
							"%s",
							UIL::LS(UIWidgetCommonStrings::XP32SkeletonWarning));

						ImGui::Spacing();
						ImGui::Separator();
					}
				}

				DrawNodeOverrideEditor(0, m_data);
			}

			ImGui::EndChild();
		}

		void UINodeOverrideEditorGlobal::EditorOnOpen()
		{
			UpdateData(m_data);
		}

		void UINodeOverrideEditorGlobal::EditorOnClose()
		{
			m_data.clear();
		}

		void UINodeOverrideEditorGlobal::EditorReset()
		{
			UpdateData(m_data);
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
			auto& settings = m_controller.GetConfigStore().settings;

			return settings.data.ui.transformEditor
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

			if (store.settings.data.ui.transformEditor.globalSex != a_newSex)
			{
				store.settings.set(
					store.settings.data.ui.transformEditor.globalSex,
					a_newSex);
			}
		}

		void UINodeOverrideEditorGlobal::ApplyProfile(
			const profileSelectorParamsNodeOverride_t<UIGlobalEditorDummyHandle>& a_data,
			const NodeOverrideProfile&                                            a_profile)
		{
			GetOrCreateConfigHolder(a_data.handle) = a_profile.Data();
			UpdateData(a_data.data);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::MergeProfile(
			const profileSelectorParamsNodeOverride_t<UIGlobalEditorDummyHandle>& a_data,
			const NodeOverrideProfile&                                            a_profile)
		{
			MergeProfileData(a_data, a_profile);
			UpdateData(a_data.data);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			UIGlobalEditorDummyHandle                      a_handle,
			const SingleNodeOverrideTransformUpdateParams& a_params)
		{
			const auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(
				a_handle,
				a_params,
				store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			UIGlobalEditorDummyHandle                      a_handle,
			const SingleNodeOverridePlacementUpdateParams& a_params)
		{
			const auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(
				a_handle,
				a_params,
				store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			UIGlobalEditorDummyHandle                    a_handle,
			const SingleNodeOverridePhysicsUpdateParams& a_params)
		{
			const auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(
				a_handle,
				a_params,
				store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnUpdate(
			UIGlobalEditorDummyHandle       a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params);

			UpdateData(a_params.data);

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnClearTransform(
			UIGlobalEditorDummyHandle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			auto& data = store.active.transforms.GetGlobalData(
				store.settings.data.ui.transformEditor.globalType);

			if (data.transformData.erase(a_params.name) > 0)
			{
				m_controller.RequestEvaluateTransformsAll(true);
			}
		}

		void UINodeOverrideEditorGlobal::OnClearPlacement(
			UIGlobalEditorDummyHandle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			auto& data = store.active.transforms.GetGlobalData(
				store.settings.data.ui.transformEditor.globalType);

			if (data.placementData.erase(a_params.name) > 0)
			{
				m_controller.RequestEvaluateTransformsAll(true);
			}
		}

		void UINodeOverrideEditorGlobal::OnClearPhysics(
			UIGlobalEditorDummyHandle            a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			auto& data = store.active.transforms.GetGlobalData(
				store.settings.data.ui.transformEditor.globalType);

			if (data.physicsData.erase(a_params.name) > 0)
			{
				m_controller.RequestEvaluateTransformsAll(true);
			}
		}

		void UINodeOverrideEditorGlobal::OnClearAllTransforms(
			UIGlobalEditorDummyHandle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			auto& data = store.active.transforms.GetGlobalData(
				store.settings.data.ui.transformEditor.globalType);

			data.transformData.clear();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnClearAllPlacement(
			UIGlobalEditorDummyHandle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			auto& data = store.active.transforms.GetGlobalData(
				store.settings.data.ui.transformEditor.globalType);

			data.placementData.clear();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		void UINodeOverrideEditorGlobal::OnClearAllPhysics(
			UIGlobalEditorDummyHandle               a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			auto& data = store.active.transforms.GetGlobalData(
				store.settings.data.ui.transformEditor.globalType);

			data.physicsData.clear();

			m_controller.RequestEvaluateTransformsAll(true);
		}

		Data::configNodeOverrideHolder_t& UINodeOverrideEditorGlobal::GetOrCreateConfigHolder(UIGlobalEditorDummyHandle) const
		{
			auto& store = m_controller.GetConfigStore();

			return store.active.transforms.GetGlobalData(
				store.settings.data.ui.transformEditor.globalType);
		}

		Data::configNodeOverrideHolder_t* UINodeOverrideEditorGlobal::GetConfigHolder(UIGlobalEditorDummyHandle) const
		{
			auto& store = m_controller.GetConfigStore();

			return std::addressof(store.active.transforms.GetGlobalData(
				store.settings.data.ui.transformEditor.globalType));
		}

		UIPopupQueue& UINodeOverrideEditorGlobal::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UINodeOverrideEditorGlobal::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		void UINodeOverrideEditorGlobal::UpdateData(entryNodeOverrideData_t& a_data)
		{
			auto& store = m_controller.GetConfigStore();

			a_data = {
				store.active.transforms.GetGlobalData(
					store.settings.data.ui.transformEditor.globalType),
				Data::ConfigClass::Global
			};
		}
	}
}