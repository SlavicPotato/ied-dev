#include "pch.h"

#include "UINodeOverrideEditorNPC.h"

#include "IED/Controller/Controller.h"
#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideEditorNPC::UINodeOverrideEditorNPC(Controller& a_controller) :
			UINodeOverrideEditorCommon<Game::FormID>(a_controller),
			UINPCList<entryNodeOverrideData_t>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UINodeOverrideEditorNPC::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformEditor.npcConfig.sex, false);
		}

		void UINodeOverrideEditorNPC::Draw()
		{
			if (ImGui::BeginChild("transform_editor_npc", { -1.0f, 0.0f }))
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
					auto disabled = entry->handle == Data::IData::GetPlayerBaseID() &&
					                !m_controller.GetNodeOverridePlayerEnabled();

					if (disabled)
					{
						DrawPlayerDisabledWarning();
					}

					UICommon::PushDisabled(disabled);

					DrawNodeOverrideEditor(entry->handle, entry->data);

					UICommon::PopDisabled(disabled);
				}
			}

			ImGui::EndChild();
		}

		void UINodeOverrideEditorNPC::OnOpen()
		{
			Reset();
		}

		void UINodeOverrideEditorNPC::OnClose()
		{
			Reset();
		}

		void UINodeOverrideEditorNPC::Reset()
		{
			ListReset();
		}

		constexpr Data::ConfigClass UINodeOverrideEditorNPC::GetConfigClass() const
		{
			return Data::ConfigClass::NPC;
		}

		const ActorInfoHolder& UINodeOverrideEditorNPC::GetActorInfoHolder() const
		{
			return m_controller.GetActorInfo();
		}

		const NPCInfoHolder& UINodeOverrideEditorNPC::GetNPCInfoHolder() const
		{
			return m_controller.GetNPCInfo();
		}

		std::uint64_t UINodeOverrideEditorNPC::GetActorInfoUpdateID() const
		{
			return m_controller.GetActorInfoUpdateID();
		}

		Data::SettingHolder::EditorPanelActorSettings& UINodeOverrideEditorNPC::GetActorSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor.npcConfig;
		}

		const SetObjectWrapper<Game::FormID>& UINodeOverrideEditorNPC::GetCrosshairRef()
		{
			return m_controller.GetCrosshairRef();
		}

		auto UINodeOverrideEditorNPC::GetCurrentData()
			-> NodeOverrideEditorCurrentData
		{
			if (auto entry = ListGetSelected())
			{
				return { entry->handle, std::addressof(entry->data) };
			}
			else
			{
				return { {}, nullptr };
			}
		}

		UIData::UICollapsibleStates& UINodeOverrideEditorNPC::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.transformEditor
			    .colStates[stl::underlying(Data::ConfigClass::NPC)];
		}

		void UINodeOverrideEditorNPC::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		Data::SettingHolder::EditorPanelCommon& UINodeOverrideEditorNPC::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor;
		}

		void UINodeOverrideEditorNPC::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UINodeOverrideEditorNPC::OnListOptionsChange()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UINodeOverrideEditorNPC::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UINodeOverrideEditorNPC::OnListChangeCurrentItem(
			const SetObjectWrapper<UINPCList<entryNodeOverrideData_t>::listValue_t>& a_oldHandle,
			const SetObjectWrapper<UINPCList<entryNodeOverrideData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& config = m_controller.GetConfigStore().settings;

			if (!config.data.ui.transformEditor.npcConfig.autoSelectSex)
			{
				return;
			}

			auto& npcInfo = m_controller.GetNPCInfo();

			auto it = npcInfo.find(a_newHandle->handle);
			if (it != npcInfo.end())
			{
				auto sex = it->second->female ? Data::ConfigSex::Female : Data::ConfigSex::Male;

				SetSex(sex, false);
			}
		}

		const entryNodeOverrideData_t& UINodeOverrideEditorNPC::GetData(Game::FormID a_handle)
		{
			auto& npcInfo = m_controller.GetNPCInfo();

			if (auto it = npcInfo.find(a_handle); it != npcInfo.end())
			{
				auto& store = m_controller.GetConfigStore();

				m_temp = store.active.transforms.GetNPC(
					a_handle,
					it->second->race);
			}
			else
			{
				m_temp.clear();
			}

			return m_temp;
		}

		void UINodeOverrideEditorNPC::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			store.settings.Set(
				store.settings.data.ui.transformEditor.npcConfig.sex,
				a_newSex);
		}

		void UINodeOverrideEditorNPC::ApplyProfile(
			profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			GetOrCreateConfigHolder(a_data.handle) = a_profile.Data();

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransformsNPC(a_data.handle, true);
		}

		void UINodeOverrideEditorNPC::MergeProfile(
			profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			auto& conf = GetOrCreateConfigHolder(a_data.handle);

			for (auto& e : a_profile.Data().data)
			{
				conf.data.insert_or_assign(e.first, e.second);
			}

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransformsNPC(a_data.handle, true);
		}

		void UINodeOverrideEditorNPC::OnUpdate(
			Game::FormID a_handle,
			const SingleNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfig(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsNPC(a_handle, true);
		}

		void UINodeOverrideEditorNPC::OnUpdate(
			Game::FormID a_handle,
			const SingleNodeOverrideParentUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfig(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsNPC(a_handle, true);
		}

		/*void UINodeOverrideEditorNPC::OnUpdate(
			Game::FormID a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			auto& conf = GetOrCreateConfigHolder(a_handle);

			conf = a_params.data;

			m_controller.RequestEvaluateTransformsNPC(a_handle, true);
		}*/

		void UINodeOverrideEditorNPC::OnClear(
			Game::FormID a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			if (EraseConfig(a_handle, data, a_params.name))
			{
				PostClear(
					GetData(a_handle).data,
					a_params.entry.data,
					a_params.name);

				m_controller.RequestEvaluateTransformsNPC(a_handle, true);
			}
		}

		void UINodeOverrideEditorNPC::OnClearParent(
			Game::FormID a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			if (EraseConfigParent(a_handle, data, a_params.name))
			{
				PostClear(
					GetData(a_handle).placementData,
					a_params.entry.placementData,
					a_params.name);

				m_controller.RequestEvaluateTransformsNPC(a_handle, true);
			}
		}

		Data::configNodeOverrideHolder_t& UINodeOverrideEditorNPC::GetOrCreateConfigHolder(Game::FormID a_handle) const
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			return data.try_emplace(a_handle).first->second;
		}

		UIPopupQueue& UINodeOverrideEditorNPC::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

	}
}