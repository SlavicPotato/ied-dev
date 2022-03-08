#include "pch.h"

#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormInfoCache.h"

#include "UISlotEditorNPC.h"

#include "IED/Controller/Controller.h"
#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		UISlotEditorNPC::UISlotEditorNPC(Controller& a_controller) :
			UISlotEditorCommon<Game::FormID>(a_controller),
			UINPCList<entrySlotData_t>(a_controller),
			UITipsInterface(a_controller),
			UINotificationInterface(a_controller),
			UILocalizationInterface(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UITransformSliderWidget(a_controller),
			m_controller(a_controller)
		{
		}

		void UISlotEditorNPC::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.slotEditor.npcConfig.sex, false);
		}

		void UISlotEditorNPC::Draw()
		{
			if (ImGui::BeginChild("slot_editor_npc", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();
				ListDraw();
				ImGui::Separator();
				ImGui::Spacing();

				if (m_listCurrent)
				{
					auto& settings = m_controller.GetConfigStore().settings;

					if (settings.data.disableNPCSlots &&
					    m_listCurrent->handle != Data::IData::GetPlayerBaseID())
					{
						DrawSlotEditorNPCWarningHeader();

						ImGui::Separator();
						ImGui::Spacing();
					}

					DrawSlotEditor(m_listCurrent->handle, m_listCurrent->data);
				}
			}

			ImGui::EndChild();
		}

		constexpr Data::ConfigClass UISlotEditorNPC::GetConfigClass() const
		{
			return Data::ConfigClass::NPC;
		}

		Data::SettingHolder::EditorPanelActorSettings& UISlotEditorNPC::GetActorSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor.npcConfig;
		}

		auto UISlotEditorNPC::GetCurrentData()
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

		Data::SettingHolder::EditorPanelCommon& UISlotEditorNPC::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor;
		}

		void UISlotEditorNPC::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.mark_dirty();
		}

		void UISlotEditorNPC::ListResetAllValues(Game::FormID a_handle) {}

		auto UISlotEditorNPC::GetData(Game::FormID a_handle)
			-> entrySlotData_t
		{
			auto& npcInfo = m_controller.GetNPCInfo();

			auto it = npcInfo.find(a_handle);
			if (it != npcInfo.end())
			{
				auto& store = m_controller.GetConfigStore().active;

				return store.slot.GetNPCCopy(
					a_handle,
					it->second->race);
			}
			else
			{
				return {};
			}
		}

		Data::configSlotHolder_t&
			UISlotEditorNPC::GetOrCreateConfigSlotHolder(Game::FormID a_handle) const
		{
			auto& store = m_controller.GetConfigStore().active;
			auto& data  = store.slot.GetNPCData();

			return data.try_emplace(a_handle).first->second;
		}

		void UISlotEditorNPC::MergeProfile(
			const profileSelectorParamsSlot_t<Game::FormID>& a_data,
			const SlotProfile&                               a_profile)
		{
			DoMerge(a_data.handle, a_profile.Data());

			a_data.data = GetData(a_data.handle);

			m_controller.QueueResetNPC(
				a_data.handle,
				ControllerUpdateFlags::kNone);
		}

		void UISlotEditorNPC::ApplyProfile(
			const profileSelectorParamsSlot_t<Game::FormID>& a_data,
			const SlotProfile&                               a_profile)
		{
			DoApply(a_data.handle, a_profile.Data());

			a_data.data = GetData(a_data.handle);

			m_controller.QueueResetNPC(
				a_data.handle,
				ControllerUpdateFlags::kNone);
		}

		void UISlotEditorNPC::OnBaseConfigChange(
			Game::FormID     a_handle,
			const void*      a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingleSlot(
				a_handle,
				params,
				store.settings.data.ui.slotEditor.sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
				m_controller.QueueEvaluateNPC(
					a_handle,
					ControllerUpdateFlags::kNone);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetNPC(
					a_handle,
					ControllerUpdateFlags::kNone,
					params->slot);
				break;
			case PostChangeAction::UpdateTransform:
				m_controller.QueueUpdateTransformSlotNPC(
					a_handle,
					params->slot);
				break;
			case PostChangeAction::AttachNode:
				m_controller.QueueAttachSlotNodeNPC(
					a_handle,
					params->slot,
					true);
				break;
			}
		}

		void UISlotEditorNPC::OnFullConfigChange(
			Game::FormID                  a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);

			a_params.data = GetData(a_handle);

			m_controller.QueueResetNPC(
				a_handle,
				ControllerUpdateFlags::kNone);
		}

		void UISlotEditorNPC::OnSingleSlotClear(
			Game::FormID                       a_handle,
			const SingleSlotConfigClearParams& a_params)
		{
			auto& store = m_controller.GetConfigStore().active;

			ResetConfigSlot(a_handle, a_params.slot, store.slot.GetNPCData());

			a_params.data = GetData(a_handle);

			m_controller.QueueResetNPC(
				a_handle,
				ControllerUpdateFlags::kNone,
				a_params.slot);
		}

		void UISlotEditorNPC::OnFullConfigClear(
			Game::FormID                     a_handle,
			const FullSlotConfigClearParams& a_params)
		{
			auto& store = m_controller.GetConfigStore().active;

			ResetConfig(a_handle, store.slot.GetNPCData());
			a_params.data = GetData(a_handle);

			m_controller.QueueResetNPC(
				a_handle,
				ControllerUpdateFlags::kNone);
		}

		void UISlotEditorNPC::OnListChangeCurrentItem(
			const stl::optional<UINPCList<entrySlotData_t>::listValue_t>& a_oldHandle,
			const stl::optional<UINPCList<entrySlotData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& settings = m_controller.GetConfigStore().settings;

			if (!settings.data.ui.slotEditor.npcConfig.autoSelectSex)
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

		void UISlotEditorNPC::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.slotEditor.npcConfig.sex != a_newSex)
			{
				store.settings.set(
					store.settings.data.ui.slotEditor.npcConfig.sex,
					a_newSex);
			}
		}

		void UISlotEditorNPC::OnListOptionsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.mark_dirty();
		}

		UIPopupQueue& UISlotEditorNPC::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UISlotEditorNPC::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		void UISlotEditorNPC::OnOpen()
		{
			Reset();
		}

		void UISlotEditorNPC::OnClose()
		{
			Reset();
		}

		void UISlotEditorNPC::Reset()
		{
			ListReset();
		}

		void UISlotEditorNPC::QueueUpdateCurrent()
		{
			QueueListUpdateCurrent();
		}

		UIData::UICollapsibleStates& UISlotEditorNPC::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			return settings.data.ui.slotEditor
			    .colStates[stl::underlying(Data::ConfigClass::NPC)];
		}

		void UISlotEditorNPC::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UISlotEditorNPC::DrawMenuBarItemsExtra()
		{
		}

		const ImVec4* UISlotEditorNPC::HighlightEntry(Game::FormID a_handle)
		{
			const auto& data = m_controller.GetConfigStore().active.slot.GetNPCData();

			if (auto it = data.find(a_handle); it != data.end() && !it->second.empty())
			{
				return std::addressof(UICommon::g_colorLightOrange);
			}

			return nullptr;
		}

		entrySlotData_t UISlotEditorNPC::GetCurrentData(Game::FormID a_handle)
		{
			return GetData(a_handle);
		}

	}
}