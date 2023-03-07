#include "pch.h"

#include "UIOutfitEditorGlobal.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		namespace OM
		{
			UIOutfitEditorGlobal::UIOutfitEditorGlobal(
				Controller& a_controller) :
				UIOutfitEntryEditorCommon<
					UIOutfitEditorParamsHandle<UIGlobalEditorDummyHandle>>(a_controller),
				UISettingsInterface(a_controller),
				UIEquipmentOverrideConditionsWidget(a_controller),
				m_controller(a_controller)
			{
			}

			UIOutfitEditorGlobal::~UIOutfitEditorGlobal() noexcept
			{
				GetProfileManager().RemoveSink(this);
			}

			void UIOutfitEditorGlobal::EditorInitialize()
			{
				InitializeProfileBase();

				const auto& settings = m_controller.GetSettings();

				SetSex(settings.data.ui.outfitEditor.globalSex, false);
			}

			void UIOutfitEditorGlobal::EditorDraw()
			{
				if (ImGui::BeginChild("otft_editor_global", { -1.0f, 0.0f }))
				{
					const UIOutfitEditorParamsHandle<UIGlobalEditorDummyHandle> params{};

					DrawOutfitEntryWrapper(m_data, params);
				}

				ImGui::EndChild();
			}

			void UIOutfitEditorGlobal::EditorOnOpen()
			{
				UpdateData();
			}

			void UIOutfitEditorGlobal::EditorOnClose()
			{
				UpdateData();
			}

			void UIOutfitEditorGlobal::EditorReset()
			{
				UpdateData();
			}

			void UIOutfitEditorGlobal::EditorQueueUpdateCurrent()
			{
				UpdateData();
			}

			constexpr Data::ConfigClass UIOutfitEditorGlobal::GetConfigClass() const
			{
				return Data::ConfigClass::Global;
			}

			Data::SettingHolder::EditorPanelCommon& UIOutfitEditorGlobal::GetEditorPanelSettings()
			{
				return m_controller.GetSettings().data.ui.outfitEditor;
			}

			void UIOutfitEditorGlobal::OnEditorPanelSettingsChange()
			{
				m_controller.GetSettings().mark_dirty();
			}

			void UIOutfitEditorGlobal::OnSexChanged(Data::ConfigSex a_newSex)
			{
				auto& settings = m_controller.GetSettings();

				if (settings.data.ui.outfitEditor.globalSex != a_newSex)
				{
					settings.set(
						settings.data.ui.outfitEditor.globalSex,
						a_newSex);
				}
			}

			UIPopupQueue& UIOutfitEditorGlobal::GetPopupQueue_ProfileBase() const
			{
				return m_controller.UIGetPopupQueue();
			}

			UIData::UICollapsibleStates& UIOutfitEditorGlobal::GetCollapsibleStatesData()
			{
				auto& settings = m_controller.GetSettings();

				return settings.data.ui.outfitEditor
				    .colStates[stl::underlying(Data::ConfigClass::Global)];
			}

			void UIOutfitEditorGlobal::OnCollapsibleStatesUpdate()
			{
				m_controller.GetSettings().mark_dirty();
			}

			entryOutfitData_t& UIOutfitEditorGlobal::GetOrCreateEntryData(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const
			{
				return m_controller.GetActiveConfig().outfit.GetGlobalData(Data::GlobalConfigType::NPC);
			}

			std::optional<std::reference_wrapper<entryOutfitData_t>> UIOutfitEditorGlobal::GetEntryData(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const
			{
				return m_controller.GetActiveConfig().outfit.GetGlobalData(Data::GlobalConfigType::NPC);
			}

			void UIOutfitEditorGlobal::OnOutfitEntryChange(
				const change_param_type& a_params)
			{
				const auto& settings = m_controller.GetSettings();

				UpdateConfig(a_params, settings.data.ui.outfitEditor.sexSync);

				m_controller.QueueEvaluateAll(
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
					ControllerUpdateFlags::kWantGroupUpdate |
					ControllerUpdateFlags::kImmediateTransformUpdate);
			}

			void UIOutfitEditorGlobal::OnOutfitEntryErase(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params)
			{
			}

			void UIOutfitEditorGlobal::ApplyProfile(
				const profileSelectorParamsOutfit_t<UIOutfitEditorParamsHandle<UIGlobalEditorDummyHandle>>& a_params,
				const OutfitProfile&                                                                        a_profile)
			{
				a_params.data                         = a_profile.Data();
				GetOrCreateEntryData(a_params.params) = a_profile.Data();

				m_controller.QueueEvaluateAll(
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
					ControllerUpdateFlags::kWantGroupUpdate |
					ControllerUpdateFlags::kImmediateTransformUpdate);
			}

			void UIOutfitEditorGlobal::UpdateData()
			{
				m_data = m_controller.GetActiveConfig().outfit.GetGlobalData(Data::GlobalConfigType::NPC);
			}

		}
	}
}