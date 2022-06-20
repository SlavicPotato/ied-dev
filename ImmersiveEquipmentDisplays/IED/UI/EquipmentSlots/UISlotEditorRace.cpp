#include "pch.h"

#include "UISlotEditorRace.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UISlotEditorRace::UISlotEditorRace(Controller& a_controller) :
			UISlotEditorCommon<Game::FormID>(a_controller),
			UIRaceList<entrySlotData_t>(a_controller),
			UITipsInterface(a_controller),
			UINotificationInterface(a_controller),
			UILocalizationInterface(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UITransformSliderWidget(a_controller),
			UIFormTypeSelectorWidget(a_controller),
			UISimpleStringSetWidget(a_controller),
			m_controller(a_controller)
		{
		}

		void UISlotEditorRace::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.slotEditor.raceConfig.sex, false);
		}

		void UISlotEditorRace::Draw()
		{
			if (ImGui::BeginChild("slot_editor_race", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();
				ListDraw();
				ImGui::Separator();
				ImGui::Spacing();

				if (m_listCurrent)
				{
					DrawSlotEditor(m_listCurrent->handle, m_listCurrent->data);
				}
			}

			ImGui::EndChild();
		}

		constexpr Data::ConfigClass UISlotEditorRace::GetConfigClass() const
		{
			return Data::ConfigClass::Race;
		}

		Data::SettingHolder::EditorPanelRaceSettings& UISlotEditorRace::GetRaceSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor.raceConfig;
		}

		auto UISlotEditorRace::GetCurrentData()
			-> SlotEditorCurrentData
		{
			if (auto& entry = ListGetSelected())
			{
				return { entry->handle, std::addressof(entry->data) };
			}
			else
			{
				return {};
			}
		}

		Data::SettingHolder::EditorPanelCommon& UISlotEditorRace::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor;
		}

		void UISlotEditorRace::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.mark_dirty();
		}

		void UISlotEditorRace::ListResetAllValues(
			Game::FormID a_handle)
		{}

		auto UISlotEditorRace::GetData(Game::FormID a_handle)
			-> entrySlotData_t
		{
			auto& store = m_controller.GetConfigStore();

			return store.active.slot.GetRaceCopy(
				a_handle,
				store.settings.data.ui.slotEditor.globalType);
		}

		auto UISlotEditorRace::GetOrCreateConfigSlotHolder(
			Game::FormID a_handle) const
			-> Data::configSlotHolder_t&
		{
			auto& store = m_controller.GetConfigStore().active;
			auto& data  = store.slot.GetRaceData();

			return data.try_emplace(a_handle).first->second;
		}

		void UISlotEditorRace::MergeProfile(
			const profileSelectorParamsSlot_t<Game::FormID>& a_data,
			const SlotProfile&                               a_profile)
		{
			DoMerge(a_data.handle, a_profile.Data());

			a_data.data = GetData(a_data.handle);

			m_controller.QueueResetRace(
				a_data.handle,
				ControllerUpdateFlags::kNone);
		}

		void UISlotEditorRace::ApplyProfile(
			const profileSelectorParamsSlot_t<Game::FormID>& a_data,
			const SlotProfile&                               a_profile)
		{
			DoApply(a_data.handle, a_profile.Data());

			a_data.data = GetData(a_data.handle);

			m_controller.QueueResetRace(
				a_data.handle,
				ControllerUpdateFlags::kNone);
		}

		void UISlotEditorRace::OnBaseConfigChange(
			Game::FormID     a_handle,
			const void*      a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(
				a_handle,
				*params,
				store.settings.data.ui.slotEditor.sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
				m_controller.QueueEvaluateRace(
					a_handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetRace(
					a_handle,
					ControllerUpdateFlags::kNone,
					params->slot);
				break;
			case PostChangeAction::UpdateTransform:
				m_controller.QueueUpdateTransformSlotRace(
					a_handle,
					params->slot);
				break;
			}
		}

		void UISlotEditorRace::OnFullConfigChange(
			Game::FormID                  a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);

			a_params.data = GetData(a_handle);

			m_controller.QueueResetRace(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorRace::OnPriorityConfigChange(
			Game::FormID                          a_handle,
			const SlotPriorityConfigUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(
				a_handle,
				a_params,
				store.settings.data.ui.slotEditor.sexSync);

			m_controller.QueueEvaluateRace(
				a_handle,
				ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
					ControllerUpdateFlags::kImmediateTransformUpdate);
		}

		void UISlotEditorRace::OnPriorityConfigClear(
			Game::FormID                  a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);

			a_params.data = GetData(a_handle);

			m_controller.QueueEvaluateRace(
				a_handle,
				ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
					ControllerUpdateFlags::kImmediateTransformUpdate);
		}

		void UISlotEditorRace::OnSingleSlotClear(
			Game::FormID                       a_handle,
			const SingleSlotConfigClearParams& a_params)
		{
			auto& store = m_controller.GetConfigStore().active;

			ResetConfigSlot(a_handle, a_params.slot, store.slot.GetRaceData());

			a_params.data = GetData(a_handle);

			m_controller.QueueResetRace(a_handle, ControllerUpdateFlags::kNone, a_params.slot);
		}

		void UISlotEditorRace::OnFullConfigClear(
			Game::FormID                     a_handle,
			const FullSlotConfigClearParams& a_params)
		{
			auto& store = m_controller.GetConfigStore().active;

			ResetConfig(a_handle, store.slot.GetRaceData());
			QueueListUpdateCurrent();

			m_controller.QueueResetRace(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorRace::OnSexChanged(
			Data::ConfigSex a_newSex)
		{
			auto& settings = m_controller.GetConfigStore().settings;

			if (settings.data.ui.slotEditor.raceConfig.sex != a_newSex)
			{
				settings.set(
					settings.data.ui.slotEditor.raceConfig.sex,
					a_newSex);
			}
		}

		void UISlotEditorRace::OnListOptionsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.mark_dirty();
		}

		UIPopupQueue& UISlotEditorRace::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UISlotEditorRace::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIData::UICollapsibleStates& UISlotEditorRace::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			return settings.data.ui.slotEditor
			    .colStates[stl::underlying(Data::ConfigClass::Race)];
		}

		void UISlotEditorRace::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		const ImVec4* UISlotEditorRace::HighlightEntry(Game::FormID a_handle)
		{
			const auto& data = m_controller.GetConfigStore().active.slot.GetRaceData();

			if (auto it = data.find(a_handle); it != data.end() && !it->second.empty())
			{
				return std::addressof(UICommon::g_colorPurple);
			}

			return nullptr;
		}

		entrySlotData_t UISlotEditorRace::GetCurrentData(Game::FormID a_handle)
		{
			return GetData(a_handle);
		}

		void UISlotEditorRace::OnOpen()
		{
			Reset();
		}

		void UISlotEditorRace::OnClose()
		{
			Reset();
		}

		void UISlotEditorRace::Reset()
		{
			ListReset();
		}

		void UISlotEditorRace::QueueUpdateCurrent()
		{
			QueueListUpdateCurrent();
		}
	}
}