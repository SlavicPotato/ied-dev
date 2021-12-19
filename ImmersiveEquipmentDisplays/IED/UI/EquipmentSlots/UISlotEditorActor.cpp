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
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{}

		void UISlotEditorActor::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.slotEditor.actorConfig.sex, false);
		}

		void UISlotEditorActor::Draw()
		{
			if (ImGui::BeginChild("slot_editor_actor", { -1.0f, 0.0f }))
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
					if (m_controller.IsActorBlockedImpl(entry->handle))
					{
						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
						ImGui::TextUnformatted(LS(UIWidgetCommonStrings::ActorBlocked));
						ImGui::PopStyleColor();
						ImGui::Spacing();
					}

					DrawSlotEditor(entry->handle, entry->data);
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
			return m_controller.GetConfigStore().settings.data.ui.slotEditor.actorConfig;
		}

		auto UISlotEditorActor::GetCurrentData() -> SlotEditorCurrentData
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

		Data::SettingHolder::EditorPanelCommon& UISlotEditorActor::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor;
		}

		void UISlotEditorActor::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.MarkDirty();
		}

		void UISlotEditorActor::ListResetAllValues(Game::FormID a_handle) {}

		auto UISlotEditorActor::GetData(Game::FormID a_handle)
			-> const entrySlotData_t&
		{
			auto& actorInfo = m_controller.GetActorInfo();

			auto it = actorInfo.find(a_handle);
			if (it == actorInfo.end())
			{
				m_tempData = {};
			}
			else
			{
				auto& store = m_controller.GetConfigStore().active;

				m_tempData = store.slot.GetActorCopy(
					a_handle,
					it->second.GetBase(),
					it->second.GetRace());
			}

			return m_tempData;
		}

		Data::configSlotHolder_t&
			UISlotEditorActor::GetOrCreateConfigSlotHolder(Game::FormID a_handle) const
		{
			auto& store = m_controller.GetConfigStore().active;
			auto& data = store.slot.GetActorData();

			return data.try_emplace(a_handle).first->second;
		}

		void UISlotEditorActor::MergeProfile(
			profileSelectorParamsSlot_t<Game::FormID>& a_data,
			const SlotProfile& a_profile)
		{
			UpdateConfigFromProfile(a_data.handle, a_profile.Data(), true);

			a_data.data = GetData(a_data.handle);

			ResetFormSelectorWidgets();

			m_controller.QueueReset(a_data.handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorActor::OnBaseConfigChange(
			Game::FormID a_handle,
			const void* a_params,
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
				m_controller.QueueEvaluate(a_handle, ControllerUpdateFlags::kNone);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone, params->slot);
				break;
			case PostChangeAction::UpdateTransform:
				m_controller.QueueUpdateTransformSlot(a_handle, params->slot);
				break;
			case PostChangeAction::AttachNode:
				m_controller.QueueAttachSlotNode(a_handle, params->slot, true);
				break;
			}
		}

		void UISlotEditorActor::OnFullConfigChange(
			Game::FormID a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);

			ResetFormSelectorWidgets();

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorActor::OnSingleSlotClear(
			Game::FormID a_handle,
			const void* a_params)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& store = m_controller.GetConfigStore().active;

			ResetConfigSlot(a_handle, params->slot, store.slot.GetActorData());
			QueueListUpdateCurrent();

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone, params->slot);
		}

		void UISlotEditorActor::OnFullConfigClear(Game::FormID a_handle)
		{
			auto& store = m_controller.GetConfigStore().active;

			ResetConfig(a_handle, store.slot.GetActorData());
			QueueListUpdateCurrent();

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorActor::OnListChangeCurrentItem(
			const SetObjectWrapper<UIActorList<entrySlotData_t>::listValue_t>& a_oldHandle,
			const SetObjectWrapper<UIActorList<entrySlotData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& config = m_controller.GetConfigStore().settings;

			if (!config.data.ui.slotEditor.actorConfig.autoSelectSex)
			{
				return;
			}

			auto& actorInfo = m_controller.GetActorInfo();

			auto it = actorInfo.find(a_newHandle->handle);
			if (it != actorInfo.end())
			{
				auto sex = it->second.GetSex();

				if (GetSex() != sex)
				{
					ResetFormSelectorWidgets();
				}

				SetSex(sex, false);
			}
		}

		void UISlotEditorActor::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.slotEditor.actorConfig.sex != a_newSex)
			{
				ResetFormSelectorWidgets();
				store.settings.Set(
					store.settings.data.ui.slotEditor.actorConfig.sex,
					a_newSex);
			}
		}

		void UISlotEditorActor::OnListOptionsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.MarkDirty();
		}

		const ActorInfoHolder& UISlotEditorActor::GetActorInfoHolder() const
		{
			return m_controller.GetActorInfo();
		}

		std::uint64_t UISlotEditorActor::GetActorInfoUpdateID() const
		{
			return m_controller.GetActorInfoUpdateID();
		}

		const SetObjectWrapper<Game::FormID>& UISlotEditorActor::GetCrosshairRef()
		{
			return m_controller.GetCrosshairRef();
		}

		UIPopupQueue& UISlotEditorActor::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UISlotEditorActor::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		void UISlotEditorActor::OnOpen()
		{
			Reset();
		}

		void UISlotEditorActor::OnClose()
		{
			Reset();
		}

		void UISlotEditorActor::Reset()
		{
			ListReset();
			ResetFormSelectorWidgets();
		}

		void UISlotEditorActor::QueueUpdateCurrent()
		{
			QueueListUpdateCurrent();
		}

		UIData::UICollapsibleStates& UISlotEditorActor::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.slotEditor
			    .colStates[stl::underlying(Data::ConfigClass::Actor)];
		}

		void UISlotEditorActor::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UISlotEditorActor::DrawMenuBarItemsExtra()
		{
			auto entry = ListGetSelected();
			if (!entry)
			{
				return;
			}

			ImGui::Separator();

			ImGui::PushID("mb_extra");

			if (ImGui::BeginMenu(entry->desc.c_str()))
			{
				if (ImGui::MenuItem(LS(CommonStrings::Evaluate, "1")))
				{
					m_controller.QueueEvaluate(entry->handle, ControllerUpdateFlags::kNone);
				}

				if (ImGui::MenuItem(LS(CommonStrings::Reset, "2")))
				{
					m_controller.QueueReset(entry->handle, ControllerUpdateFlags::kNone);
				}

				ImGui::Separator();

				if (ImGui::MenuItem(LS(UIWidgetCommonStrings::NiNodeUpdate, "3")))
				{
					m_controller.QueueNiNodeUpdate(entry->handle);
				}

				ImGui::EndMenu();
			}

			ImGui::PopID();
		}

		void UISlotEditorActor::ListDrawExtraActorInfo(
			listValue_t* a_entry)
		{
			auto& data = m_controller.GetObjects();

			auto it = data.find(a_entry->handle);
			if (it == data.end())
			{
				return;
			}

			if (!it->second.AnySlotOccupied())
			{
				return;
			}

			ImGui::Spacing();
			ImGui::Text("%s:", LS(UIWidgetCommonStrings::OccupiedSlots));
			ImGui::Spacing();

			ImGui::Indent();

			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			for (enum_type j = 0; j < stl::underlying(Data::ObjectSlot::kMax); j++)
			{
				auto slotId = static_cast<Data::ObjectSlot>(j);
				auto& slot = it->second.GetSlot(slotId);

				if (!slot.state)
				{
					continue;
				}

				auto name = Data::GetSlotName(slotId);

				std::stringstream ss;

				ss << std::left << std::setfill(' ') << std::setw(22) << name << " ["
				   << sshex(8) << *slot.state->item << "] [" << *slot.state->nodeDesc.name << "]";

				if (slot.state->flags.test(ObjectEntryFlags::kScbLeft))
				{
					ss << " [ScbLeft]";
				}

				if (!slot.state->nodes.obj->IsVisible())
				{
					ss << " ";
					ss << L(UIWidgetCommonStrings::HiddenBrackets);
				}

				ss << std::endl;

				ImGui::TextWrapped("%s", ss.str().c_str());
			}

			ImGui::Unindent();
		}

		bool UISlotEditorActor::DrawExtraSlotInfo(
			Game::FormID a_handle,
			Data::ObjectSlot a_slot,
			const Data::configStoreSlot_t::result_copy::result_entry& a_entry,
			bool a_infoDrawn)
		{
			auto& data = m_controller.GetObjects();

			auto it = data.find(a_handle);
			if (it == data.end())
			{
				return false;
			}

			auto& slot = it->second.GetSlot(a_slot);

			if (!slot.state)
			{
				return false;
			}

			if (a_infoDrawn)
			{
				ImGui::SameLine(0.0f, 25.0f);
			}

			ImGui::Text("%s:", LS(CommonStrings::Item));
			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorLightBlue);

			auto& flc = m_controller.UIGetFormLookupCache();

			if (auto formInfo = flc.LookupForm(*slot.state->item))
			{
				if (!slot.state->nodes.obj->IsVisible())
				{
					ImGui::TextColored(
						UICommon::g_colorGreyed,
						"%s",
						LS(UIWidgetCommonStrings::HiddenBrackets));
					ImGui::SameLine();
				}

				if (formInfo->form.name.empty())
				{
					ImGui::Text("%.8X", formInfo->form.id.get());
				}
				else
				{
					ImGui::Text("%s", formInfo->form.name.c_str());
				}

				ImGui::PopStyleColor();

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);

					ImGui::TextUnformatted(LS(UIWidgetCommonStrings::FormIDColon));
					ImGui::SameLine();
					ImGui::Text("%.8X", formInfo->form.id.get());

					if (auto typeDesc = IFormCommon::GetFormTypeDesc(formInfo->form.type))
					{
						ImGui::TextUnformatted(LS(UIWidgetCommonStrings::TypeColon));
						ImGui::SameLine();
						ImGui::Text("%s", typeDesc);
					}
					else
					{
						ImGui::TextUnformatted(LS(UIWidgetCommonStrings::TypeColon));
						ImGui::SameLine();
						ImGui::Text("%hhu", formInfo->form.type);
					}

					if (slot.state->flags.test(ObjectEntryFlags::kScbLeft))
					{
						ImGui::Spacing();
						ImGui::TextUnformatted(LS(UIWidgetCommonStrings::LeftScbAttached));
					}

					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
			}
			else
			{
				ImGui::Text("%.8X", slot.state->item.get());
				if (slot.state->flags.test(ObjectEntryFlags::kScbLeft))
				{
					UICommon::HelpMarker(LS(UIWidgetCommonStrings::LeftScbAttached));
				}

				ImGui::PopStyleColor();
			}

			return true;
		}
	}
}