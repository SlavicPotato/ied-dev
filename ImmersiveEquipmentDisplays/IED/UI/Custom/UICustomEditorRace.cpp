#include "pch.h"

#include "UICustomEditorRace.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UICustomEditorRace::UICustomEditorRace(
			Controller& a_controller) :
			UICustomEditorCommon<Game::FormID>(a_controller),
			UIRaceList<entryCustomData_t>(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UIPopupInterface(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			m_controller(a_controller)
		{
		}

		UICustomEditorRace::~UICustomEditorRace() noexcept
		{
			GlobalProfileManager::GetSingleton<CustomProfile>().RemoveSink(this);
		}

		void UICustomEditorRace::EditorInitialize()
		{
			InitializeProfileBase();

			const auto& settings = m_controller.GetSettings();

			SetSex(settings.data.ui.customEditor.raceConfig.sex, false);
		}

		void UICustomEditorRace::EditorDraw()
		{
			if (ImGui::BeginChild("custom_editor_race", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();
				ListDraw();
				ImGui::Separator();
				ImGui::Spacing();

				if (m_listCurrent)
				{
					DrawCustomEditor(m_listCurrent->handle, m_listCurrent->data);
				}
			}

			ImGui::EndChild();
		}

		void UICustomEditorRace::EditorOnOpen()
		{
			EditorReset();
		}

		void UICustomEditorRace::EditorOnClose()
		{
			EditorReset();
		}

		void UICustomEditorRace::EditorReset()
		{
			ListReset();
		}

		void UICustomEditorRace::EditorQueueUpdateCurrent()
		{
			QueueListUpdateCurrent();
		}

		constexpr Data::ConfigClass UICustomEditorRace::GetConfigClass() const
		{
			return Data::ConfigClass::Race;
		}

		Data::SettingHolder::EditorPanelRaceSettings& UICustomEditorRace::GetRaceSettings() const
		{
			return m_controller
			    .GetSettings()
			    .data.ui.customEditor.raceConfig;
		}

		Data::configCustomHolder_t& UICustomEditorRace::GetOrCreateConfigSlotHolder(
			Game::FormID a_handle) const
		{
			auto& data = m_controller.GetActiveConfig().custom.GetRaceData();
			auto& sh   = StringHolder::GetSingleton();

			auto& pluginMap = data.try_emplace(a_handle).first->second;

			return pluginMap.try_emplace(sh.IED).first->second;
		}

		entryCustomData_t UICustomEditorRace::GetData(Game::FormID a_handle)
		{
			auto& data  = m_controller.GetActiveConfig().custom.GetRaceData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				auto& sh = StringHolder::GetSingleton();

				auto it2 = it->second.find(sh.IED);
				if (it2 != it->second.end())
				{
					return it2->second;
				}
			}

			return {};
		}

		auto UICustomEditorRace::GetCurrentData()
			-> CustomEditorCurrentData
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

		UIPopupQueue& UICustomEditorRace::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UICustomEditorRace::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIData::UICollapsibleStates& UICustomEditorRace::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetSettings();

			return settings
			    .data.ui.customEditor
			    .colStates[stl::underlying(Data::ConfigClass::Race)];
		}

		void UICustomEditorRace::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UICustomEditorRace::OnListOptionsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		Data::SettingHolder::EditorPanelCommon& UICustomEditorRace::GetEditorPanelSettings()
		{
			return m_controller
			    .GetSettings()
			    .data.ui.customEditor;
		}

		void UICustomEditorRace::OnEditorPanelSettingsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UICustomEditorRace::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UICustomEditorRace::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& settings = m_controller.GetSettings();

			if (settings.data.ui.customEditor.raceConfig.sex != a_newSex)
			{
				settings.set(
					settings.data.ui.customEditor.raceConfig.sex,
					a_newSex);
			}
		}

		void UICustomEditorRace::ApplyProfile(
			const profileSelectorParamsCustom_t<Game::FormID>& a_data,
			const CustomProfile&                               a_profile)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_data.handle);

			a_data.data = a_profile.Data();
			conf        = a_profile.Data();

			m_controller.QueueResetCustomRace(
				a_data.handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorRace::MergeProfile(
			const profileSelectorParamsCustom_t<Game::FormID>& a_data,
			const CustomProfile&                               a_profile)
		{
			auto& profileData = a_profile.Data();

			for (auto& [i, e] : profileData.data)
			{
				a_data.data.data.insert_or_assign(i, e);
			}

			GetOrCreateConfigSlotHolder(a_data.handle) = a_data.data;

			m_controller.QueueResetCustomRace(
				a_data.handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorRace::OnBaseConfigChange(
			Game::FormID     a_handle,
			const void*      a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleCustomConfigUpdateParams*>(a_params);

			auto& settings = m_controller.GetSettings();

			UpdateConfig(a_handle, *params, settings.data.ui.customEditor.sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
			case PostChangeAction::UpdateTransform:
				m_controller.QueueEvaluateRace(
					a_handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kWantGroupUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetCustomRace(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					params->name);
				break;
			}
		}

		void UICustomEditorRace::OnFullConfigChange(
			Game::FormID                    a_handle,
			const CustomConfigUpdateParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			conf = a_params.data;

			m_controller.QueueResetCustomRace(
				a_handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		bool UICustomEditorRace::OnCreateNew(
			Game::FormID                 a_handle,
			const CustomConfigNewParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			auto r = conf
			             .data.try_emplace(
								  a_params.name,
								  a_params.entry)
			             .second;

			if (r)
			{
				m_controller.QueueEvaluateRace(a_handle, ControllerUpdateFlags::kNone);
			}

			return r;
		}

		void UICustomEditorRace::OnErase(
			Game::FormID                   a_handle,
			const CustomConfigEraseParams& a_params)
		{
			auto& data = m_controller
			                 .GetActiveConfig()
			                 .custom.GetRaceData();

			if (EraseConfig(a_handle, data, a_params.name))
			{
				m_controller.QueueResetCustomRace(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					a_params.name);
			}
		}

		bool UICustomEditorRace::OnRename(
			Game::FormID                    a_handle,
			const CustomConfigRenameParams& a_params)
		{
			if (!DoConfigRename(a_handle, a_params))
			{
				return false;
			}

			m_controller.QueueResetCustomRace(
				a_handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED,
				a_params.oldName);

			return true;
		}

		const ImVec4* UICustomEditorRace::HighlightEntry(Game::FormID a_handle)
		{
			return HasConfigEntry(
					   m_controller
						   .GetActiveConfig()
						   .custom.GetRaceData(),
					   a_handle) ?
			           std::addressof(UICommon::g_colorPurple) :
                       nullptr;
		}
	}
}
