#include "pch.h"

#include "UICustomEditorRace.h"

#include "../Widgets/UIWidgetsCommon.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UICustomEditorRace::UICustomEditorRace(
			Controller& a_controller) :
			UICustomEditorCommon<Game::FormID>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UICustomEditorRace::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.customEditor.raceConfig.sex, false);
		}

		void UICustomEditorRace::Draw()
		{
			if (ImGui::BeginChild("custom_editor_race", { -1.0f, 0.0f }))
			{
				ListTick();

				auto entry = ListGetSelected();
				const char* curSelName{ nullptr };

				ImGui::Spacing();
				ListDraw(entry, curSelName);
				ImGui::Separator();
				ImGui::Spacing();

				if (entry)
				{
					DrawCustomEditor(entry->handle, entry->data);
				}
			}

			ImGui::EndChild();
		}

		void UICustomEditorRace::OnOpen()
		{
			Reset();
		}

		void UICustomEditorRace::OnClose()
		{
			Reset();
		}

		void UICustomEditorRace::Reset()
		{
			ListReset();
			ResetFormSelectorWidgets();
		}

		void UICustomEditorRace::QueueUpdateCurrent()
		{
			QueueListUpdateCurrent();
		}

		constexpr Data::ConfigClass UICustomEditorRace::GetConfigClass() const
		{
			return Data::ConfigClass::Race;
		}

		const ActorInfoHolder& UICustomEditorRace::GetActorInfoHolder() const
		{
			return m_controller.GetActorInfo();
		}

		Data::SettingHolder::EditorPanelRaceSettings& UICustomEditorRace::GetRaceSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.customEditor.raceConfig;
		}

		Data::configCustomHolder_t& UICustomEditorRace::GetOrCreateConfigSlotHolder(
			Game::FormID a_handle) const
		{
			auto& data = m_controller.GetConfigStore().active.custom.GetRaceData();
			auto& sh = StringHolder::GetSingleton();

			auto& pluginMap = data.try_emplace(a_handle).first->second;

			return pluginMap.try_emplace(sh.IED).first->second;
		}

		const entryCustomData_t& UICustomEditorRace::GetData(Game::FormID a_handle)
		{
			auto& store = m_controller.GetConfigStore();
			auto& data = store.active.custom.GetRaceData();

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

			return m_empty;
		}

		auto UICustomEditorRace::GetCurrentData()
			-> CustomEditorCurrentData
		{
			auto entry = ListGetSelected();
			if (entry)
			{
				return { entry->handle, std::addressof(entry->data) };
			}
			else
			{
				return { {}, nullptr };
			}
		}

		const SetObjectWrapper<Game::FormID>& UICustomEditorRace::GetCrosshairRef()
		{
			return m_controller.GetCrosshairRef();
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
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.customEditor
			    .colStates[stl::underlying(Data::ConfigClass::Race)];
		}

		void UICustomEditorRace::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UICustomEditorRace::OnListOptionsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.MarkDirty();
		}

		Data::SettingHolder::EditorPanelCommon& UICustomEditorRace::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.customEditor;
		}

		void UICustomEditorRace::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.MarkDirty();
		}

		void UICustomEditorRace::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UICustomEditorRace::OnListChangeCurrentItem(
			const SetObjectWrapper<UIRaceList<entryCustomData_t>::listValue_t>& a_oldHandle,
			const SetObjectWrapper<UIRaceList<entryCustomData_t>::listValue_t>& a_newHandle)
		{
			ResetFormSelectorWidgets();
		}

		void UICustomEditorRace::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.customEditor.raceConfig.sex != a_newSex)
			{
				ResetFormSelectorWidgets();
				store.settings.Set(
					store.settings.data.ui.customEditor.raceConfig.sex,
					a_newSex);
			}
		}

		void UICustomEditorRace::ApplyProfile(
			profileSelectorParamsCustom_t<Game::FormID>& a_data,
			const CustomProfile& a_profile)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_data.handle);

			a_data.data = a_profile.Data();
			conf = a_profile.Data();

			ResetFormSelectorWidgets();

			m_controller.QueueResetCustomRace(
				a_data.handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorRace::MergeProfile(
			profileSelectorParamsCustom_t<Game::FormID>& a_data,
			const CustomProfile& a_profile)
		{
			auto& profileData = a_profile.Data();

			for (auto& e : profileData.data)
			{
				a_data.data.data.insert_or_assign(e.first, e.second);
			}

			GetOrCreateConfigSlotHolder(a_data.handle) = a_data.data;

			ResetFormSelectorWidgets();

			m_controller.QueueResetCustomRace(
				a_data.handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorRace::OnBaseConfigChange(
			Game::FormID a_handle,
			const void* a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleCustomConfigUpdateParams*>(a_params);

			auto& store = m_controller.GetConfigStore();

			UpdateConfig(a_handle, *params, store.settings.data.ui.customEditor.sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
				m_controller.QueueEvaluateRace(
					a_handle,
					ControllerUpdateFlags::kNone);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetCustomRace(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					params->name);
				break;
			case PostChangeAction::UpdateTransform:
				m_controller.QueueUpdateTransformCustomRace(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					params->name);
				break;
			case PostChangeAction::AttachNode:
				m_controller.QueueUpdateAttachCustomRace(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					params->name);
				break;
			}
		}

		void UICustomEditorRace::OnFullConfigChange(
			Game::FormID a_handle,
			const CustomConfigUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			conf = a_params.data;

			ResetFormSelectorWidgets();

			m_controller.QueueResetCustomRace(
				a_handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		bool UICustomEditorRace::OnCreateNew(
			Game::FormID a_handle,
			const CustomConfigNewParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			auto r = conf.data.try_emplace(a_params.name, a_params.entry).second;

			if (r)
			{
				m_controller.QueueEvaluateRace(a_handle, ControllerUpdateFlags::kNone);
			}

			return r;
		}

		void UICustomEditorRace::OnErase(
			Game::FormID a_handle,
			const CustomConfigEraseParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.custom.GetRaceData();

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
			Game::FormID a_handle,
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
	}
}
