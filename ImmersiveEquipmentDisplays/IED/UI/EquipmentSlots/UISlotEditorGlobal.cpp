#include "pch.h"

#include "UISlotEditorGlobal.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UISlotEditorGlobal::UISlotEditorGlobal(Controller& a_controller) :
			UISlotEditorCommon<int>(a_controller, true),
			UITipsInterface(a_controller),
			UINotificationInterface(a_controller),
			UILocalizationInterface(a_controller),
			UISettingsInterface(a_controller),
			UIGlobalConfigTypeSelectorWidget(a_controller),
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
				auto& settings = m_controller.GetConfigStore().settings;

				if (DrawTypeSelectorRadio(settings.data.ui.slotEditor.globalType))
				{
					settings.mark_dirty();
					UpdateData(m_data);
				}

				ImGui::Separator();
				ImGui::Spacing();

				bool sep = false;

				if (settings.data.ui.slotEditor.globalType == Data::GlobalConfigType::Player &&
					m_controller.IsActorBlockedImpl(Data::IData::GetPlayerRefID()))
				{
					ImGui::TextColored(
						UICommon::g_colorWarning,
						"%s",
						LS(UIWidgetCommonStrings::ActorBlocked));

					ImGui::Spacing();

					sep = true;
				}

				if (settings.data.disableNPCSlots &&
				    settings.data.ui.slotEditor.globalType == Data::GlobalConfigType::NPC)
				{
					DrawSlotEditorNPCWarningHeader();

					sep = true;
				}

				if (sep)
				{
					ImGui::Separator();
					ImGui::Spacing();
				}

				DrawSlotEditor(0, m_data);
			}

			ImGui::EndChild();
		}

		bool UISlotEditorGlobal::ShowConfigClassIndicator() const
		{
			return false;
		}

		bool UISlotEditorGlobal::PermitDeletion() const
		{
			return false;
		}

		constexpr Data::ConfigClass UISlotEditorGlobal::GetConfigClass() const
		{
			return Data::ConfigClass::Global;
		}

		auto UISlotEditorGlobal::GetCurrentData()
			-> SlotEditorCurrentData
		{
			return { 0, std::addressof(m_data) };
		}

		Data::SettingHolder::EditorPanelCommon& UISlotEditorGlobal::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.slotEditor;
		}

		void UISlotEditorGlobal::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		Data::configSlotHolder_t& UISlotEditorGlobal::GetOrCreateConfigSlotHolder(int) const
		{
			auto& store = m_controller.GetConfigStore().active;

			const auto& settings = m_controller.GetConfigStore().settings;

			return store.slot.GetGlobalData(settings.data.ui.slotEditor.globalType);
		}

		void UISlotEditorGlobal::MergeProfile(
			const profileSelectorParamsSlot_t<int>& a_data,
			const SlotProfile&                      a_profile)
		{
			DoMerge(a_data.handle, a_profile.Data());

			UpdateData(a_data.data);

			m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
		}

		/*void UISlotEditorGlobal::ApplyProfile(
			const profileSelectorParamsSlot_t<int>& a_data,
			const SlotProfile& a_profile)
		{
			const auto& settings = m_controller.GetConfigStore().settings;
			auto& dconf = m_controller
			                  .GetConfigStore()
			                  .initial.slot
			                  .GetGlobalData(settings.data.ui.slotEditor.globalType);

			UpdateConfigFromProfile(a_data.handle, a_profile.Data(), true, std::addressof(dconf));

			UpdateData(a_data.data);

			m_controller.QueueResetAll(ControllerUpdateFlags::kNone);
		}*/

		void UISlotEditorGlobal::OnBaseConfigChange(
			int              a_handle,
			const void*      a_params,
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
			int                           a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);
			UpdateData(a_params.data);

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorGlobal::OnSingleSlotClear(
			int,
			const SingleSlotConfigClearParams&)
		{
		}

		void UISlotEditorGlobal::OnFullConfigClear(
			int,
			const FullSlotConfigClearParams&)
		{
		}

		void UISlotEditorGlobal::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.slotEditor.globalSex != a_newSex)
			{
				store.settings.set(store.settings.data.ui.slotEditor.globalSex, a_newSex);
			}
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
			auto& settings = m_controller.GetConfigStore().settings;

			return settings.data.ui.slotEditor
			    .colStates[stl::underlying(Data::ConfigClass::Global)];
		}

		void UISlotEditorGlobal::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UISlotEditorGlobal::UpdateData(entrySlotData_t& a_data)
		{
			auto& store = m_controller.GetConfigStore();

			const auto& settings = store.settings;

			a_data = {
				store.active.slot.GetGlobalData(
					settings.data.ui.slotEditor.globalType),
				Data::ConfigClass::Global
			};
		}

		entrySlotData_t UISlotEditorGlobal::GetCurrentData(int)
		{
			return m_data;
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
			UpdateData(m_data);
		}

		void UISlotEditorGlobal::QueueUpdateCurrent()
		{
			UpdateData(m_data);
		}
	}
}