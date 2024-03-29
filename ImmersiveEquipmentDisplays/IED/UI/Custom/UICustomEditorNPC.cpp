#include "pch.h"

#include "UICustomEditorNPC.h"

#include "../Widgets/UIWidgetsCommon.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UICustomEditorNPC::UICustomEditorNPC(
			Controller& a_controller) :
			UICustomEditorCommon<Game::FormID>(a_controller),
			UINPCList<entryCustomData_t>(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UIPopupInterface(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			m_controller(a_controller)
		{
		}

		UICustomEditorNPC::~UICustomEditorNPC() noexcept
		{
			GlobalProfileManager::GetSingleton<CustomProfile>().RemoveSink(this);
		}

		void UICustomEditorNPC::EditorInitialize()
		{
			InitializeProfileBase();

			const auto& settings = m_controller.GetSettings();

			SetSex(settings.data.ui.customEditor.npcConfig.sex, false);
		}

		void UICustomEditorNPC::EditorDraw()
		{
			if (ImGui::BeginChild("custom_editor_npc", { -1.0f, 0.0f }))
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

		void UICustomEditorNPC::EditorOnOpen()
		{
			EditorReset();
		}

		void UICustomEditorNPC::EditorOnClose()
		{
			EditorReset();
		}

		void UICustomEditorNPC::EditorReset()
		{
			ListReset();
		}

		void UICustomEditorNPC::EditorQueueUpdateCurrent()
		{
			QueueListUpdateCurrent();
		}

		constexpr Data::ConfigClass UICustomEditorNPC::GetConfigClass() const
		{
			return Data::ConfigClass::NPC;
		}

		Data::SettingHolder::EditorPanelActorSettings& UICustomEditorNPC::GetActorSettings() const
		{
			return m_controller.GetSettings().data.ui.customEditor.npcConfig;
		}

		Data::configCustomHolder_t& UICustomEditorNPC::GetOrCreateConfigSlotHolder(
			Game::FormID a_handle) const
		{
			auto& data = m_controller.GetActiveConfig().custom.GetNPCData();
			auto& sh   = StringHolder::GetSingleton();

			auto& pluginMap = data.try_emplace(a_handle).first->second;

			return pluginMap.try_emplace(sh.IED).first->second;
		}

		entryCustomData_t UICustomEditorNPC::GetData(Game::FormID a_handle)
		{
			auto& store = m_controller.GetActiveConfig();
			auto& data  = store.custom.GetNPCData();

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

		auto UICustomEditorNPC::GetCurrentData()
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

		UIPopupQueue& UICustomEditorNPC::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UICustomEditorNPC::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIData::UICollapsibleStates& UICustomEditorNPC::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetSettings();

			return settings.data.ui.customEditor
			    .colStates[stl::underlying(Data::ConfigClass::NPC)];
		}

		void UICustomEditorNPC::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UICustomEditorNPC::OnListOptionsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		Data::SettingHolder::EditorPanelCommon& UICustomEditorNPC::GetEditorPanelSettings()
		{
			return m_controller.GetSettings().data.ui.customEditor;
		}

		void UICustomEditorNPC::OnEditorPanelSettingsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UICustomEditorNPC::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UICustomEditorNPC::OnListChangeCurrentItem(
			const std::optional<UINPCList<entryCustomData_t>::listValue_t>& a_oldHandle,
			const std::optional<UINPCList<entryCustomData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& settings = m_controller.GetSettings();

			if (!settings.data.ui.customEditor.actorConfig.autoSelectSex)
			{
				return;
			}

			auto& npcInfo = m_controller.GetNPCInfo();

			auto it = npcInfo.find(a_newHandle->handle);
			if (it != npcInfo.end())
			{
				auto sex = it->second->female ?
				               Data::ConfigSex::Female :
                               Data::ConfigSex::Male;

				SetSex(sex, false);
			}
		}

		void UICustomEditorNPC::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& settings = m_controller.GetSettings();

			if (settings.data.ui.customEditor.npcConfig.sex != a_newSex)
			{
				settings.set(
					settings.data.ui.customEditor.npcConfig.sex,
					a_newSex);
			}
		}

		void UICustomEditorNPC::ApplyProfile(
			const profileSelectorParamsCustom_t<Game::FormID>& a_data,
			const CustomProfile&                               a_profile)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_data.handle);

			a_data.data = a_profile.Data();
			conf        = a_profile.Data();

			m_controller.QueueResetCustomNPC(
				a_data.handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorNPC::MergeProfile(
			const profileSelectorParamsCustom_t<Game::FormID>& a_data,
			const CustomProfile&                               a_profile)
		{
			auto& profileData = a_profile.Data();

			for (auto& [i, e] : profileData.data)
			{
				a_data.data.data.insert_or_assign(i, e);
			}

			GetOrCreateConfigSlotHolder(a_data.handle) = a_data.data;

			m_controller.QueueResetCustomNPC(
				a_data.handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorNPC::OnBaseConfigChange(
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
				m_controller.QueueEvaluateNPC(
					a_handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kWantGroupUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetCustomNPC(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					params->name);
				break;
			}
		}

		void UICustomEditorNPC::OnFullConfigChange(
			Game::FormID                    a_handle,
			const CustomConfigUpdateParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			conf = a_params.data;

			m_controller.QueueResetCustomNPC(
				a_handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		bool UICustomEditorNPC::OnCreateNew(
			Game::FormID                 a_handle,
			const CustomConfigNewParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			auto r = conf.data.try_emplace(a_params.name, a_params.entry).second;

			if (r)
			{
				m_controller.QueueEvaluateNPC(a_handle, ControllerUpdateFlags::kNone);
			}

			return r;
		}

		void UICustomEditorNPC::OnErase(
			Game::FormID                   a_handle,
			const CustomConfigEraseParams& a_params)
		{
			auto& data = m_controller.GetActiveConfig().custom.GetNPCData();

			if (EraseConfig(a_handle, data, a_params.name))
			{
				m_controller.QueueResetCustomNPC(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					a_params.name);
			}
		}

		bool UICustomEditorNPC::OnRename(
			Game::FormID                    a_handle,
			const CustomConfigRenameParams& a_params)
		{
			if (!DoConfigRename(a_handle, a_params))
			{
				return false;
			}

			m_controller.QueueResetCustomNPC(
				a_handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED,
				a_params.oldName);

			return true;
		}

		const ImVec4* UICustomEditorNPC::HighlightEntry(Game::FormID a_handle)
		{
			return HasConfigEntry(
					   m_controller.GetActiveConfig().custom.GetNPCData(),
					   a_handle) ?
			           std::addressof(UICommon::g_colorLightOrange) :
                       nullptr;
		}
	}
}
