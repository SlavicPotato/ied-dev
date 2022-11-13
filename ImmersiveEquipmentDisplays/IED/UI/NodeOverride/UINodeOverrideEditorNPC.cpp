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
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UITransformSliderWidget(a_controller),
			m_controller(a_controller)
		{
		}

		void UINodeOverrideEditorNPC::EditorInitialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformEditor.npcConfig.sex, false);
		}

		void UINodeOverrideEditorNPC::EditorDraw()
		{
			if (ImGui::BeginChild("no_editor_npc", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();
				ListDraw();
				ImGui::Separator();
				ImGui::Spacing();

				if (m_listCurrent)
				{
					auto disabled = m_listCurrent->handle == Data::IData::GetPlayerBaseID() &&
					                !m_controller.GetNodeOverridePlayerEnabled();

					if (disabled)
					{
						DrawPlayerDisabledWarning();
					}

					UICommon::PushDisabled(disabled);

					DrawNodeOverrideEditor(m_listCurrent->handle, m_listCurrent->data);

					UICommon::PopDisabled(disabled);
				}
			}

			ImGui::EndChild();
		}

		void UINodeOverrideEditorNPC::EditorOnOpen()
		{
			EditorReset();
		}

		void UINodeOverrideEditorNPC::EditorOnClose()
		{
			EditorReset();
		}

		void UINodeOverrideEditorNPC::EditorReset()
		{
			ListReset();
		}

		constexpr Data::ConfigClass UINodeOverrideEditorNPC::GetConfigClass() const
		{
			return Data::ConfigClass::NPC;
		}

		Data::SettingHolder::EditorPanelActorSettings& UINodeOverrideEditorNPC::GetActorSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor.npcConfig;
		}

		auto UINodeOverrideEditorNPC::GetCurrentData()
			-> NodeOverrideEditorCurrentData
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

		UIData::UICollapsibleStates& UINodeOverrideEditorNPC::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			return settings.data.ui.transformEditor
			    .colStates[stl::underlying(Data::ConfigClass::NPC)];
		}

		void UINodeOverrideEditorNPC::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		Data::SettingHolder::EditorPanelCommon& UINodeOverrideEditorNPC::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor;
		}

		void UINodeOverrideEditorNPC::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UINodeOverrideEditorNPC::OnListOptionsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UINodeOverrideEditorNPC::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UINodeOverrideEditorNPC::OnListChangeCurrentItem(
			const stl::optional<UINPCList<entryNodeOverrideData_t>::listValue_t>& a_oldHandle,
			const stl::optional<UINPCList<entryNodeOverrideData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& settings = m_controller.GetConfigStore().settings;

			if (!settings.data.ui.transformEditor.npcConfig.autoSelectSex)
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

		entryNodeOverrideData_t UINodeOverrideEditorNPC::GetData(Game::FormID a_handle)
		{
			auto& npcInfo = m_controller.GetNPCInfo();

			if (auto it = npcInfo.find(a_handle); it != npcInfo.end())
			{
				auto& store = m_controller.GetConfigStore();

				return store.active.transforms.GetNPCCopy(
					a_handle,
					it->second->race);
			}
			else
			{
				return {};
			}
		}

		void UINodeOverrideEditorNPC::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.transformEditor.npcConfig.sex != a_newSex)
			{
				store.settings.set(
					store.settings.data.ui.transformEditor.npcConfig.sex,
					a_newSex);
			}
		}

		void UINodeOverrideEditorNPC::ApplyProfile(
			const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile&                               a_profile)
		{
			GetOrCreateConfigHolder(a_data.handle) = a_profile.Data();

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransformsNPC(a_data.handle, true);
		}

		void UINodeOverrideEditorNPC::MergeProfile(
			const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile&                               a_profile)
		{
			MergeProfileData(a_data, a_profile);

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransformsNPC(a_data.handle, true);
		}

		void UINodeOverrideEditorNPC::OnUpdate(
			Game::FormID                                   a_handle,
			const SingleNodeOverrideTransformUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsNPC(a_handle, true);
		}

		void UINodeOverrideEditorNPC::OnUpdate(
			Game::FormID                                   a_handle,
			const SingleNodeOverridePlacementUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsNPC(a_handle, true);
		}

		void UINodeOverrideEditorNPC::OnUpdate(
			Game::FormID                                 a_handle,
			const SingleNodeOverridePhysicsUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsNPC(a_handle, true);
		}

		void UINodeOverrideEditorNPC::OnUpdate(
			Game::FormID                    a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params);

			a_params.data = GetData(a_handle);

			m_controller.RequestEvaluateTransformsNPC(a_handle, true);
		}

		void UINodeOverrideEditorNPC::OnClearTransform(
			Game::FormID                         a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			if (EraseConfig<
					Data::configNodeOverrideEntryTransform_t>(
					a_handle,
					data,
					a_params.name))
			{
				m_controller.RequestEvaluateTransformsNPC(a_handle, true);
			}

			PostClear(
				GetData(a_handle).transformData,
				a_params.entry.transformData,
				a_params.name);
		}

		void UINodeOverrideEditorNPC::OnClearPlacement(
			Game::FormID                         a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			if (EraseConfig<
					Data::configNodeOverrideEntryPlacement_t>(
					a_handle,
					data,
					a_params.name))
			{
				m_controller.RequestEvaluateTransformsNPC(a_handle, true);
			}

			PostClear(
				GetData(a_handle).placementData,
				a_params.entry.placementData,
				a_params.name);
		}

		void UINodeOverrideEditorNPC::OnClearPhysics(Game::FormID a_handle, const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			if (EraseConfig<
					Data::configNodeOverrideEntryPhysics_t>(
					a_handle,
					data,
					a_params.name))
			{
				m_controller.RequestEvaluateTransformsNPC(a_handle, true);
			}

			PostClear(
				GetData(a_handle).physicsData,
				a_params.entry.physicsData,
				a_params.name);
		}

		void UINodeOverrideEditorNPC::OnClearAllTransforms(
			Game::FormID                            a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				it->second.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);
				it->second.transformData.clear();

				m_controller.RequestEvaluateTransformsNPC(a_handle, true);
			}

			a_params.entry.transformData = GetData(a_handle).transformData;
		}

		void UINodeOverrideEditorNPC::OnClearAllPlacement(
			Game::FormID                            a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				it->second.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);
				it->second.placementData.clear();

				m_controller.RequestEvaluateTransformsNPC(a_handle, true);
			}

			a_params.entry.placementData = GetData(a_handle).placementData;
		}

		void UINodeOverrideEditorNPC::OnClearAllPhysics(
			Game::FormID                            a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				it->second.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);
				it->second.physicsData.clear();

				m_controller.RequestEvaluateTransformsNPC(a_handle, true);
			}

			a_params.entry.physicsData = GetData(a_handle).physicsData;
		}

		Data::configNodeOverrideHolder_t& UINodeOverrideEditorNPC::GetOrCreateConfigHolder(Game::FormID a_handle) const
		{
			auto& result = m_controller
			                   .GetConfigStore()
			                   .active.transforms.GetNPCData()
			                   .try_emplace(a_handle)
			                   .first->second;

			result.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);

			return result;
		}

		Data::configNodeOverrideHolder_t* UINodeOverrideEditorNPC::GetConfigHolder(Game::FormID a_handle) const
		{
			auto& data = m_controller
			                 .GetConfigStore()
			                 .active.transforms.GetNPCData();

			auto it = data.find(a_handle);

			return it != data.end() ? std::addressof(it->second) : nullptr;
		}

		UIPopupQueue& UINodeOverrideEditorNPC::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UINodeOverrideEditorNPC::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		const ImVec4* UINodeOverrideEditorNPC::HighlightEntry(Game::FormID a_handle)
		{
			const auto& data = m_controller.GetConfigStore().active.transforms.GetNPCData();

			if (auto it = data.find(a_handle); it != data.end() && !it->second.empty())
			{
				return std::addressof(UICommon::g_colorLightOrange);
			}

			return nullptr;
		}

	}
}