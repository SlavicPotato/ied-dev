#include "pch.h"

#include "UISlotEditorGlobal.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UISlotEditorGlobal::UISlotEditorGlobal(Controller& a_controller) :
			UISlotEditorCommon<int>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UISlotEditorGlobal::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.slotEditor.globalSex, false);
		}

		void UISlotEditorGlobal::Draw()
		{
			if (ImGui::BeginChild("slot_editor_global", { -1.0f, 0.0f }))
			{
				DrawTypeSelectorRadio();
				ImGui::Separator();
				ImGui::Spacing();

				DrawSlotEditor(0, m_data);
			}

			ImGui::EndChild();
		}

		bool UISlotEditorGlobal::ShowConfigClassIndicator() const { return false; }

		bool UISlotEditorGlobal::PermitDeletion() const { return false; }

		constexpr Data::ConfigClass UISlotEditorGlobal::GetConfigClass() const
		{
			return Data::ConfigClass::Global;
		}

		auto UISlotEditorGlobal::GetCurrentData() -> SlotEditorCurrentData
		{
			return { 0, std::addressof(m_data) };
		}

		Data::SettingHolder::EditorPanelCommon& UISlotEditorGlobal::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor;
		}

		void UISlotEditorGlobal::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.MarkDirty();
		}

		Data::configSlotHolder_t&
			UISlotEditorGlobal::GetOrCreateConfigSlotHolder(int) const
		{
			auto& store = m_controller.GetConfigStore().active;
			auto& data = store.slot.GetGlobalData();

			const auto& config = m_controller.GetConfigStore().settings;

			return data[stl::underlying(config.data.ui.slotEditor.globalType)];
		}

		void UISlotEditorGlobal::MergeProfile(
			profileSelectorParamsSlot_t<int>& a_data,
			const SlotProfile& a_profile)
		{
			UpdateConfigFromProfile(a_data.handle, a_profile.Data(), false);

			UpdateData();

			m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
		}

		void UISlotEditorGlobal::OnBaseConfigChange(
			int a_handle,
			const void* a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingleSlot(a_handle, params, GetEditorPanelSettings().sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
				m_controller.QueueEvaluateAll(ControllerUpdateFlags::kNone);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetAll(ControllerUpdateFlags::kNone, params->slot);
				break;
			case PostChangeAction::UpdateTransform:
				m_controller.QueueUpdateTransformSlotAll(params->slot);
				break;
			case PostChangeAction::AttachNode:
				m_controller.QueueAttachSlotNodeAll(params->slot, true);
				break;
			}
		}

		void UISlotEditorGlobal::OnFullConfigChange(
			int a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorGlobal::OnSingleSlotClear(int a_handle, const void* a_params)
		{
		}

		void UISlotEditorGlobal::OnFullConfigClear(int a_handle) {}

		void UISlotEditorGlobal::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.slotEditor.globalSex != a_newSex)
			{
				ResetFormSelectorWidgets();
				store.settings.Set(store.settings.data.ui.slotEditor.globalSex, a_newSex);
			}
		}

		void UISlotEditorGlobal::DrawTypeSelectorRadio()
		{
			auto& config = m_controller.GetConfigStore().settings;

			ImGui::PushID("__type_selector");

			if (ImGui::RadioButton(
					"Player",
					config.data.ui.slotEditor.globalType ==
						Data::GlobalConfigType::Player))
			{
				config.Set(
					config.data.ui.slotEditor.globalType,
					Data::GlobalConfigType::Player);
				UpdateData();
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					"NPCs",
					config.data.ui.slotEditor.globalType ==
						Data::GlobalConfigType::NPC))
			{
				config.Set(
					config.data.ui.slotEditor.globalType,
					Data::GlobalConfigType::NPC);
				UpdateData();
			}

			ImGui::PopID();
		}

		UIPopupQueue& UISlotEditorGlobal::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UISlotEditorGlobal::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIData::UICollapsibleStates& UISlotEditorGlobal::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.slotEditor
			    .colStates[stl::underlying(Data::ConfigClass::Global)];
		}

		void UISlotEditorGlobal::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UISlotEditorGlobal::UpdateData()
		{
			auto& store = m_controller.GetConfigStore();

			const auto& config = store.settings;

			m_data = store.active.slot.GetGlobalCopy(config.data.ui.slotEditor.globalType);

			ResetFormSelectorWidgets();
		}

		void UISlotEditorGlobal::OnOpen()
		{
			Reset();
		}

		void UISlotEditorGlobal::OnClose()
		{
			m_data = {};
		}

		void UISlotEditorGlobal::Reset()
		{
			UpdateData();
		}

		void UISlotEditorGlobal::QueueUpdateCurrent()
		{
			UpdateData();
		}
	}  // namespace UI
}  // namespace IED