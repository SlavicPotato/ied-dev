#include "pch.h"

#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormInfoCache.h"

#include "UISlotEditorActor.h"

#include "IED/Controller/Controller.h"
#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		UISlotEditorActor::UISlotEditorActor(Controller& a_controller) :
			UISlotEditorCommon<Game::FormID>(a_controller),
			UIActorList<entrySlotData_t>(a_controller),
			UIPopupInterface(a_controller),
			UIFormInfoTooltipWidget(a_controller),
			UIMiscTextInterface(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			m_controller(a_controller)
		{
		}

		UISlotEditorActor::~UISlotEditorActor() noexcept
		{
			GetProfileManager().RemoveSink(this);
		}

		void UISlotEditorActor::EditorInitialize()
		{
			InitializeProfileBase();

			const auto& settings = m_controller.GetSettings();

			SetSex(settings.data.ui.slotEditor.actorConfig.sex, false);
		}

		void UISlotEditorActor::EditorDraw()
		{
			if (ImGui::BeginChild("slot_editor_actor", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();
				ListDraw();
				ImGui::Separator();
				ImGui::Spacing();

				if (m_listCurrent)
				{
					bool sep = false;

					if (m_controller.IsActorBlockedImpl(m_listCurrent->handle))
					{
						ImGui::TextColored(
							UICommon::g_colorWarning,
							"%s",
							UIL::LS(UIWidgetCommonStrings::ActorBlocked));

						ImGui::Spacing();

						sep = true;
					}

					if (!m_controller.SkeletonCheck(m_listCurrent->handle))
					{
						ImGui::TextColored(
							UICommon::g_colorWarning,
							"%s",
							UIL::LS(UIWidgetCommonStrings::XP32SkeletonWarning));

						ImGui::Spacing();

						sep = true;
					}

					auto& settings = m_controller.GetSettings();

					if (settings.data.disableNPCSlots &&
					    m_listCurrent->handle != Data::IData::GetPlayerRefID())
					{
						DrawSlotEditorNPCWarningHeader();

						sep = true;
					}

					if (sep)
					{
						ImGui::Separator();
						ImGui::Spacing();
					}

					DrawSlotEditor(m_listCurrent->handle, m_listCurrent->data);
				}
			}

			ImGui::EndChild();
		}

		constexpr Data::ConfigClass UISlotEditorActor::GetConfigClass() const
		{
			return Data::ConfigClass::Actor;
		}

		Data::SettingHolder::EditorPanelActorSettings& UISlotEditorActor::GetActorSettings() const
		{
			return m_controller.GetSettings().data.ui.slotEditor.actorConfig;
		}

		auto UISlotEditorActor::GetCurrentData() -> SlotEditorCurrentData
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

		Data::SettingHolder::EditorPanelCommon& UISlotEditorActor::GetEditorPanelSettings()
		{
			return m_controller.GetSettings().data.ui.slotEditor;
		}

		void UISlotEditorActor::OnEditorPanelSettingsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UISlotEditorActor::ListResetAllValues(Game::FormID a_handle) {}

		auto UISlotEditorActor::GetData(Game::FormID a_handle)
			-> entrySlotData_t
		{
			auto& actorInfo = m_controller.GetActorInfo();

			auto it = actorInfo.find(a_handle);
			if (it != actorInfo.end())
			{
				auto& store = m_controller.GetActiveConfig();

				return store.slot.GetActorCopy(
					a_handle,
					it->second.GetBaseOrTemplate(),
					it->second.GetRace());
			}
			else
			{
				return {};
			}
		}

		Data::configSlotHolder_t& UISlotEditorActor::GetOrCreateConfigSlotHolder(Game::FormID a_handle) const
		{
			auto& data = m_controller
			                 .GetActiveConfig()
			                 .slot.GetActorData();

			return data.try_emplace(a_handle).first->second;
		}

		void UISlotEditorActor::MergeProfile(
			const profileSelectorParamsSlot_t<Game::FormID>& a_data,
			const SlotProfile&                               a_profile)
		{
			DoMerge(a_data.handle, a_profile.Data());

			a_data.data = GetData(a_data.handle);

			m_controller.QueueReset(a_data.handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorActor::ApplyProfile(
			const profileSelectorParamsSlot_t<Game::FormID>& a_data,
			const SlotProfile&                               a_profile)
		{
			DoApply(a_data.handle, a_profile.Data());

			a_data.data = GetData(a_data.handle);

			m_controller.QueueReset(a_data.handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorActor::OnBaseConfigChange(
			Game::FormID     a_handle,
			const void*      a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& settings = m_controller.GetSettings();

			UpdateConfigSingle(
				a_handle,
				*params,
				settings.data.ui.slotEditor.sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
				m_controller.QueueEvaluate(
					a_handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueReset(
					a_handle,
					ControllerUpdateFlags::kNone,
					params->slot);
				break;
			case PostChangeAction::UpdateTransform:
				m_controller.QueueUpdateTransformSlot(
					a_handle,
					params->slot);
				break;
			}
		}

		void UISlotEditorActor::OnFullConfigChange(
			Game::FormID                  a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);

			a_params.data = GetData(a_handle);

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorActor::OnPriorityConfigChange(
			Game::FormID                          a_handle,
			const SlotPriorityConfigUpdateParams& a_params)
		{
			const auto& settings = m_controller.GetSettings();

			UpdateConfigSingle(
				a_handle,
				a_params,
				settings.data.ui.slotEditor.sexSync);

			m_controller.QueueEvaluate(
				a_handle,
				ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
					ControllerUpdateFlags::kImmediateTransformUpdate);
		}

		void UISlotEditorActor::OnPriorityConfigClear(
			Game::FormID                  a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);

			a_params.data = GetData(a_handle);

			m_controller.QueueEvaluate(
				a_handle,
				ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
					ControllerUpdateFlags::kImmediateTransformUpdate);
		}

		void UISlotEditorActor::OnSingleSlotClear(
			Game::FormID                       a_handle,
			const SingleSlotConfigClearParams& a_params)
		{
			auto& store = m_controller.GetActiveConfig();

			ResetConfigSlot(a_handle, a_params.slot, store.slot.GetActorData());
			a_params.data = GetData(a_handle);

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone, a_params.slot);
		}

		void UISlotEditorActor::OnFullConfigClear(
			Game::FormID                     a_handle,
			const FullSlotConfigClearParams& a_params)
		{
			auto& store = m_controller.GetActiveConfig();

			ResetConfig(a_handle, store.slot.GetActorData());
			a_params.data = GetData(a_handle);

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorActor::OnListChangeCurrentItem(
			const std::optional<UIActorList<entrySlotData_t>::listValue_t>& a_oldHandle,
			const std::optional<UIActorList<entrySlotData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& settings = m_controller.GetSettings();

			if (!settings.data.ui.slotEditor.actorConfig.autoSelectSex)
			{
				return;
			}

			auto& actorInfo = m_controller.GetActorInfo();

			auto it = actorInfo.find(a_newHandle->handle);
			if (it != actorInfo.end())
			{
				SetSex(it->second.GetSex(), false);
			}
		}

		void UISlotEditorActor::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& settings = m_controller.GetSettings();

			if (settings.data.ui.slotEditor.actorConfig.sex != a_newSex)
			{
				settings.set(
					settings.data.ui.slotEditor.actorConfig.sex,
					a_newSex);
			}
		}

		void UISlotEditorActor::OnListOptionsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		UIPopupQueue& UISlotEditorActor::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UISlotEditorActor::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		void UISlotEditorActor::EditorOnOpen()
		{
			EditorReset();
		}

		void UISlotEditorActor::EditorOnClose()
		{
			EditorReset();
		}

		void UISlotEditorActor::EditorReset()
		{
			ListReset();
		}

		void UISlotEditorActor::EditorQueueUpdateCurrent()
		{
			QueueListUpdateCurrent();
		}

		UIData::UICollapsibleStates& UISlotEditorActor::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetSettings();

			return settings.data.ui.slotEditor
			    .colStates[stl::underlying(Data::ConfigClass::Actor)];
		}

		void UISlotEditorActor::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UISlotEditorActor::DrawMenuBarItemsExtra()
		{
			auto& entry = ListGetSelected();
			if (!entry)
			{
				return;
			}

			ImGui::Separator();

			ImGui::PushID("mb_extra");

			if (ImGui::BeginMenu(entry->desc.c_str()))
			{
				if (ImGui::MenuItem(UIL::LS(CommonStrings::Evaluate, "1")))
				{
					m_controller.QueueEvaluate(entry->handle, ControllerUpdateFlags::kNone);
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Reset, "2")))
				{
					m_controller.QueueReset(entry->handle, ControllerUpdateFlags::kNone);
				}

				ImGui::Separator();

				if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::NiNodeUpdate, "3")))
				{
					m_controller.QueueNiNodeUpdate(entry->handle);
				}

				ImGui::EndMenu();
			}

			ImGui::PopID();
		}

		void UISlotEditorActor::ListDrawExtraActorInfo(
			const listValue_t& a_entry)
		{
			auto& data = m_controller.GetActorMap();

			auto it = data.find(a_entry.handle);
			if (it == data.end())
			{
				return;
			}

			if (!it->second.IsAnySlotOccupied())
			{
				return;
			}

			ImGui::Spacing();
			ImGui::Text("%s:", UIL::LS(UIWidgetCommonStrings::OccupiedSlots));
			ImGui::Spacing();

			ImGui::Indent();

			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			for (enum_type j = 0; j < stl::underlying(Data::ObjectSlot::kMax); j++)
			{
				auto  slotId = static_cast<Data::ObjectSlot>(j);
				auto& slot   = it->second.GetSlot(slotId);

				if (!slot.data.state)
				{
					continue;
				}

				auto name = Data::GetSlotName(slotId);

				std::stringstream ss;

				ss << std::left << std::setfill(' ') << std::setw(22) << name << " ["
				   << sshex(8) << slot.data.state->form->formID.get() << "] [" << *slot.data.state->nodeDesc.name << "]";

				if (slot.data.state->flags.test(ObjectEntryFlags::kScbLeft))
				{
					ss << " [ScbLeft]";
				}

				if (!slot.IsNodeVisible())
				{
					ss << " ";
					ss << UIL::L(UIWidgetCommonStrings::HiddenBrackets);
				}

				ss << std::endl;

				ImGui::TextWrapped("%s", ss.str().c_str());
			}

			ImGui::Unindent();
		}

		bool UISlotEditorActor::DrawExtraSlotInfo(
			Game::FormID                      a_handle,
			Data::ObjectSlot                  a_slot,
			const entrySlotData_t::data_type& a_entry,
			bool                              a_infoDrawn)
		{
			auto& data = m_controller.GetActorMap();

			auto it = data.find(a_handle);
			if (it == data.end())
			{
				return false;
			}

			auto& slot = it->second.GetSlot(a_slot);

			if (!slot.data.state)
			{
				return false;
			}

			if (a_infoDrawn)
			{
				ImGui::SameLine(0.0f, 25.0f);
			}

			ImGui::Text("%s:", UIL::LS(CommonStrings::Item));
			ImGui::SameLine();

			DrawObjectEntryHeaderInfo(
				m_controller.UIGetFormLookupCache().LookupForm(slot.data.state->form->formID),
				slot);

			return true;
		}

		const ImVec4* UISlotEditorActor::HighlightEntry(Game::FormID a_handle)
		{
			const auto& data = m_controller.GetActiveConfig().slot.GetActorData();

			if (auto it = data.find(a_handle); it != data.end() && !it->second.empty())
			{
				return std::addressof(UICommon::g_colorLimeGreen);
			}

			return nullptr;
		}

		entrySlotData_t UISlotEditorActor::GetCurrentData(Game::FormID a_handle)
		{
			return GetData(a_handle);
		}
	}
}