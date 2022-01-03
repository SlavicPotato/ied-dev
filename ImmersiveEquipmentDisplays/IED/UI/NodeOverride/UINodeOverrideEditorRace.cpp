#include "pch.h"

#include "UINodeOverrideEditorRace.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideEditorRace::UINodeOverrideEditorRace(Controller& a_controller) :
			UINodeOverrideEditorCommon<Game::FormID>(a_controller),
			UIRaceList<entryNodeOverrideData_t>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UINodeOverrideEditorRace::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformEditor.raceConfig.sex, false);
		}

		void UINodeOverrideEditorRace::Draw()
		{
			if (ImGui::BeginChild("no_editor_race", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();
				ListDraw();
				ImGui::Separator();
				ImGui::Spacing();

				if (m_listCurrent)
				{
					DrawNodeOverrideEditor(m_listCurrent->handle, m_listCurrent->data);
				}
			}

			ImGui::EndChild();
		}

		void UINodeOverrideEditorRace::OnOpen()
		{
			Reset();
		}

		void UINodeOverrideEditorRace::OnClose()
		{
			Reset();
		}

		void UINodeOverrideEditorRace::Reset()
		{
			ListReset();
		}

		constexpr Data::ConfigClass UINodeOverrideEditorRace::GetConfigClass() const
		{
			return Data::ConfigClass::Race;
		}

		Data::SettingHolder::EditorPanelRaceSettings& UINodeOverrideEditorRace::GetRaceSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor.raceConfig;
		}

		auto UINodeOverrideEditorRace::GetCurrentData()
			-> NodeOverrideEditorCurrentData
		{
			if (auto& entry = ListGetSelected())
			{
				return { entry->handle, std::addressof(entry->data) };
			}
			else
			{
				return { {}, nullptr };
			}
		}

		UIData::UICollapsibleStates& UINodeOverrideEditorRace::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.transformEditor
			    .colStates[stl::underlying(Data::ConfigClass::Race)];
		}

		void UINodeOverrideEditorRace::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		Data::SettingHolder::EditorPanelCommon& UINodeOverrideEditorRace::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor;
		}

		void UINodeOverrideEditorRace::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UINodeOverrideEditorRace::OnListOptionsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UINodeOverrideEditorRace::ListResetAllValues(Game::FormID a_handle)
		{
		}

		entryNodeOverrideData_t UINodeOverrideEditorRace::GetData(Game::FormID a_handle)
		{
			auto& store = m_controller.GetConfigStore();

			return store.active.transforms.GetRaceCopy(
				a_handle,
				store.settings.data.ui.transformEditor.globalType);
		}

		void UINodeOverrideEditorRace::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			store.settings.set(
				store.settings.data.ui.transformEditor.raceConfig.sex,
				a_newSex);
		}

		void UINodeOverrideEditorRace::ApplyProfile(
			const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			GetOrCreateConfigHolder(a_data.handle) = a_profile.Data();

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransformsRace(a_data.handle, true);
		}

		void UINodeOverrideEditorRace::MergeProfile(
			const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			MergeProfileData(a_data, a_profile);

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransformsRace(a_data.handle, true);
		}

		void UINodeOverrideEditorRace::OnUpdate(
			Game::FormID a_handle,
			const SingleNodeOverrideTransformUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsRace(a_handle, true);
		}

		void UINodeOverrideEditorRace::OnUpdate(
			Game::FormID a_handle,
			const SingleNodeOverridePlacementUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsRace(a_handle, true);
		}

		void UINodeOverrideEditorRace::OnUpdate(
			Game::FormID a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params);

			a_params.data = GetData(a_handle);

			m_controller.RequestEvaluateTransformsRace(a_handle, true);
		}

		void UINodeOverrideEditorRace::OnClearTransform(
			Game::FormID a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetRaceData();

			if (EraseConfig<Data::configNodeOverrideEntryTransform_t>(a_handle, data, a_params.name))
			{
				m_controller.RequestEvaluateTransformsRace(a_handle, true);
			}

			PostClear(
				GetData(a_handle).data,
				a_params.entry.data,
				a_params.name);
		}

		void UINodeOverrideEditorRace::OnClearPlacement(
			Game::FormID a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetRaceData();

			if (EraseConfig<Data::configNodeOverrideEntryPlacement_t>(a_handle, data, a_params.name))
			{
				m_controller.RequestEvaluateTransformsRace(a_handle, true);
			}

			PostClear(
				GetData(a_handle).placementData,
				a_params.entry.placementData,
				a_params.name);
		}

		void UINodeOverrideEditorRace::OnClearAllTransforms(
			Game::FormID a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetRaceData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				it->second.data.clear();
				m_controller.RequestEvaluateTransformsRace(a_handle, true);
			}

			a_params.entry.data = GetData(a_handle).data;
		}

		void UINodeOverrideEditorRace::OnClearAllPlacement(
			Game::FormID a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetRaceData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				it->second.placementData.clear();

				m_controller.RequestEvaluateTransformsRace(a_handle, true);
			}

			a_params.entry.placementData = GetData(a_handle).placementData;
		}

		Data::configNodeOverrideHolder_t& UINodeOverrideEditorRace::GetOrCreateConfigHolder(Game::FormID a_handle) const
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetRaceData();

			return data.try_emplace(a_handle).first->second;
		}

		UIPopupQueue& UINodeOverrideEditorRace::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UINodeOverrideEditorRace::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		const ImVec4* UINodeOverrideEditorRace::HighlightEntry(Game::FormID a_handle)
		{
			const auto& data = m_controller.GetConfigStore().active.transforms.GetRaceData();

			if (auto it = data.find(a_handle); it != data.end() && !it->second.empty())
			{
				return std::addressof(UICommon::g_colorPurple);
			}

			return nullptr;
		}

	}
}