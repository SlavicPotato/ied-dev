#include "pch.h"

#include "UISlotEditorGlobal.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UISlotEditorGlobal::UISlotEditorGlobal(Controller& a_controller) :
			UISlotEditorCommon<UIGlobalEditorDummyHandle>(a_controller, true),
			UIPopupInterface(a_controller),
			UISettingsInterface(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			m_controller(a_controller)
		{
		}

		UISlotEditorGlobal::~UISlotEditorGlobal() noexcept
		{
			GetProfileManager().RemoveSink(this);
		}

		void UISlotEditorGlobal::EditorInitialize()
		{
			InitializeProfileBase();

			const auto& settings = m_controller.GetSettings();

			SetSex(settings.data.ui.slotEditor.globalSex, false);
		}

		void UISlotEditorGlobal::EditorDraw()
		{
			if (ImGui::BeginChild("slot_editor_global", { -1.0f, 0.0f }))
			{
				auto& settings = m_controller.GetSettings();

				if (DrawTypeSelectorRadio(settings.data.ui.slotEditor.globalType))
				{
					settings.mark_dirty();
					UpdateData(m_data);
				}

				ImGui::Separator();
				ImGui::Spacing();

				bool sep = false;

				if (settings.data.ui.slotEditor.globalType == Data::GlobalConfigType::Player)
				{
					if (m_controller.IsActorBlockedImpl(Data::IData::GetPlayerRefID()))
					{
						ImGui::TextColored(
							UICommon::g_colorWarning,
							"%s",
							UIL::LS(UIWidgetCommonStrings::ActorBlocked));

						ImGui::Spacing();

						sep = true;
					}

					if (!m_controller.SkeletonCheck(Data::IData::GetPlayerRefID()))
					{
						ImGui::TextColored(
							UICommon::g_colorWarning,
							"%s",
							UIL::LS(UIWidgetCommonStrings::XP32SkeletonWarning));

						ImGui::Spacing();

						sep = true;
					}
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
			return m_controller.GetSettings().data.ui.slotEditor;
		}

		void UISlotEditorGlobal::OnEditorPanelSettingsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		Data::configSlotHolder_t& UISlotEditorGlobal::GetOrCreateConfigSlotHolder(UIGlobalEditorDummyHandle) const
		{
			auto&       store    = m_controller.GetActiveConfig();
			const auto& settings = m_controller.GetSettings();

			return store.slot.GetGlobalData(settings.data.ui.slotEditor.globalType);
		}

		void UISlotEditorGlobal::MergeProfile(
			const profileSelectorParamsSlot_t<UIGlobalEditorDummyHandle>& a_data,
			const SlotProfile&                                            a_profile)
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
			UIGlobalEditorDummyHandle a_handle,
			const void*               a_params,
			PostChangeAction          a_action)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			UpdateConfigSingle(a_handle, *params, GetEditorPanelSettings().sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
			case PostChangeAction::UpdateTransform:
				m_controller.QueueEvaluateAll(
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
					ControllerUpdateFlags::kWantGroupUpdate |
					ControllerUpdateFlags::kImmediateTransformUpdate);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetAll(ControllerUpdateFlags::kNone, params->slot);
				break;
			}
		}

		void UISlotEditorGlobal::OnFullConfigChange(
			UIGlobalEditorDummyHandle     a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params.data);
			UpdateData(a_params.data);

			m_controller.QueueReset(a_handle, ControllerUpdateFlags::kNone);
		}

		void UISlotEditorGlobal::OnPriorityConfigChange(
			UIGlobalEditorDummyHandle             a_handle,
			const SlotPriorityConfigUpdateParams& a_params)
		{
			UpdateConfigSingle(
				a_handle,
				a_params,
				GetEditorPanelSettings().sexSync);

			m_controller.QueueEvaluateAll(
				ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
				ControllerUpdateFlags::kImmediateTransformUpdate);
		}

		void UISlotEditorGlobal::OnPriorityConfigClear(
			UIGlobalEditorDummyHandle     a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			UpdateConfig(
				a_handle,
				a_params.data);

			UpdateData(a_params.data);

			m_controller.QueueEvaluateAll(
				ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
				ControllerUpdateFlags::kImmediateTransformUpdate);
		}

		void UISlotEditorGlobal::OnSingleSlotClear(
			UIGlobalEditorDummyHandle,
			const SingleSlotConfigClearParams&)
		{
		}

		void UISlotEditorGlobal::OnFullConfigClear(
			UIGlobalEditorDummyHandle,
			const FullSlotConfigClearParams&)
		{
		}

		void UISlotEditorGlobal::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& settings = m_controller.GetSettings();

			if (settings.data.ui.slotEditor.globalSex != a_newSex)
			{
				settings.set(
					settings.data.ui.slotEditor.globalSex,
					a_newSex);
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
			auto& settings = m_controller.GetSettings();

			return settings.data.ui.slotEditor
			    .colStates[stl::underlying(Data::ConfigClass::Global)];
		}

		void UISlotEditorGlobal::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UISlotEditorGlobal::UpdateData(entrySlotData_t& a_data)
		{
			auto&       store    = m_controller.GetActiveConfig();
			const auto& settings = m_controller.GetSettings();

			a_data = {
				store.slot.GetGlobalData(
					settings.data.ui.slotEditor.globalType),
				Data::ConfigClass::Global
			};
		}

		entrySlotData_t UISlotEditorGlobal::GetCurrentData(UIGlobalEditorDummyHandle)
		{
			return m_data;
		}

		void UISlotEditorGlobal::EditorOnOpen()
		{
			UpdateData(m_data);
		}

		void UISlotEditorGlobal::EditorOnClose()
		{
			m_data = {};
		}

		void UISlotEditorGlobal::EditorReset()
		{
			UpdateData(m_data);
		}

		void UISlotEditorGlobal::EditorQueueUpdateCurrent()
		{
			UpdateData(m_data);
		}
	}
}