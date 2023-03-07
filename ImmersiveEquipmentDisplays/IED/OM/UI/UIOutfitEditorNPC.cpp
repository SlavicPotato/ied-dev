#include "pch.h"

#include "UIOutfitEditorNPC.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		namespace OM
		{
			UIOutfitEditorNPC::UIOutfitEditorNPC(
				Controller& a_controller) :
				UIOutfitEntryEditorCommon<
					UIOutfitEditorParamsHandle<Game::FormID>>(a_controller),
				UINPCList<UIOutfitEntryListItemWrapper>(a_controller),
				UISettingsInterface(a_controller),
				UIActorInfoInterface(a_controller),
				UIEquipmentOverrideConditionsWidget(a_controller),
				m_controller(a_controller)
			{
			}

			UIOutfitEditorNPC::~UIOutfitEditorNPC() noexcept
			{
				GetProfileManager().RemoveSink(this);
			}

			void UIOutfitEditorNPC::EditorInitialize()
			{
				InitializeProfileBase();

				const auto& settings = m_controller.GetSettings();

				SetSex(settings.data.ui.outfitEditor.npcConfig.sex, false);
			}

			void UIOutfitEditorNPC::EditorDraw()
			{
				if (ImGui::BeginChild("otft_editor_npc", { -1.0f, 0.0f }))
				{
					ImGui::Spacing();
					ListDraw();
					ImGui::Separator();
					ImGui::Spacing();

					if (m_listCurrent)
					{
						const UIOutfitEditorParamsHandle params{
							m_listCurrent->handle
						};

						DrawOutfitEntryWrapper(m_listCurrent->data, params);
					}
				}

				ImGui::EndChild();
			}

			void UIOutfitEditorNPC::EditorOnOpen()
			{
				EditorReset();
			}

			void UIOutfitEditorNPC::EditorOnClose()
			{
				EditorReset();
			}

			void UIOutfitEditorNPC::EditorReset()
			{
				ListReset();
			}

			void UIOutfitEditorNPC::EditorQueueUpdateCurrent()
			{
				QueueListUpdateCurrent();
			}

			constexpr Data::ConfigClass UIOutfitEditorNPC::GetConfigClass() const
			{
				return Data::ConfigClass::NPC;
			}

			Data::SettingHolder::EditorPanelActorSettings& UIOutfitEditorNPC::GetActorSettings() const
			{
				return m_controller.GetSettings().data.ui.outfitEditor.npcConfig;
			}

			Data::SettingHolder::EditorPanelCommon& UIOutfitEditorNPC::GetEditorPanelSettings()
			{
				return m_controller.GetSettings().data.ui.outfitEditor;
			}

			void UIOutfitEditorNPC::OnEditorPanelSettingsChange()
			{
				m_controller.GetSettings().mark_dirty();
			}

			UIOutfitEntryListItemWrapper UIOutfitEditorNPC::GetData(Game::FormID a_handle)
			{
				const auto& store = m_controller.GetActiveConfig().outfit;
				const auto& data  = store.GetNPCData();

				auto it2 = data.find(a_handle);
				if (it2 != data.end())
				{
					return it2->second;
				}

				auto& npcInfo = m_controller.GetNPCInfo();

				auto it = npcInfo.find(a_handle);
				if (it != npcInfo.end())
				{
					return {
						UIOutfitEntryListItemWrapper::nodata_arg_t{},
						store.GetNPC(a_handle, it->second->race)
					};
				}
				else
				{
					return {};
				}
			}

			void UIOutfitEditorNPC::OnListChangeCurrentItem(
				const std::optional<UINPCList<UIOutfitEntryListItemWrapper>::listValue_t>& a_oldHandle,
				const std::optional<UINPCList<UIOutfitEntryListItemWrapper>::listValue_t>& a_newHandle)
			{
				if (!a_newHandle)
				{
					return;
				}

				auto& settings = m_controller.GetSettings();

				if (!settings.data.ui.outfitEditor.npcConfig.autoSelectSex)
				{
					return;
				}

				auto& npcInfo = m_controller.GetNPCInfo();

				auto it = npcInfo.find(a_newHandle->handle);
				if (it != npcInfo.end())
				{
					const auto sex = it->second->female ?
					                     Data::ConfigSex::Female :
					                     Data::ConfigSex::Male;

					SetSex(sex, false);
				}
			}

			void UIOutfitEditorNPC::OnSexChanged(Data::ConfigSex a_newSex)
			{
				auto& settings = m_controller.GetSettings();

				if (settings.data.ui.outfitEditor.npcConfig.sex != a_newSex)
				{
					settings.set(
						settings.data.ui.outfitEditor.npcConfig.sex,
						a_newSex);
				}
			}

			void UIOutfitEditorNPC::OnListOptionsChange()
			{
				m_controller.GetSettings().mark_dirty();
			}

			UIPopupQueue& UIOutfitEditorNPC::GetPopupQueue_ProfileBase() const
			{
				return m_controller.UIGetPopupQueue();
			}

			UIData::UICollapsibleStates& UIOutfitEditorNPC::GetCollapsibleStatesData()
			{
				auto& settings = m_controller.GetSettings();

				return settings.data.ui.outfitEditor
				    .colStates[stl::underlying(Data::ConfigClass::NPC)];
			}

			void UIOutfitEditorNPC::OnCollapsibleStatesUpdate()
			{
				m_controller.GetSettings().mark_dirty();
			}

			void UIOutfitEditorNPC::ListResetAllValues(Game::FormID a_handle)
			{
			}

			entryOutfitData_t& UIOutfitEditorNPC::GetOrCreateEntryData(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const
			{
				auto& data = m_controller.GetActiveConfig().outfit.GetNPCData();

				return data.try_emplace(a_params.handle).first->second;
			}

			std::optional<std::reference_wrapper<entryOutfitData_t>> UIOutfitEditorNPC::GetEntryData(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const
			{
				auto& data = m_controller.GetActiveConfig().outfit.GetNPCData();

				auto it = data.find(a_params.handle);
				if (it != data.end())
				{
					return it->second;
				}

				return {};
			}

			void UIOutfitEditorNPC::OnOutfitEntryChange(
				const change_param_type& a_params)
			{
				const auto& settings = m_controller.GetSettings();

				UpdateConfig(a_params, settings.data.ui.outfitEditor.sexSync);

				if (a_params.params.noData)
				{
					QueueListUpdateCurrent();
				}

				m_controller.QueueEvaluateNPC(
					a_params.params.actual.handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kWantGroupUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
			}

			void UIOutfitEditorNPC::OnOutfitEntryErase(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params)
			{
				auto& data = m_controller.GetActiveConfig().outfit.GetNPCData();

				if (data.erase(a_params.handle))
				{
					QueueListUpdateCurrent();

					m_controller.QueueEvaluateNPC(
						a_params.handle,
						ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
							ControllerUpdateFlags::kWantGroupUpdate |
							ControllerUpdateFlags::kImmediateTransformUpdate);
				}
			}

			void UIOutfitEditorNPC::ApplyProfile(
				const profileSelectorParamsOutfit_t<UIOutfitEditorParamsHandle<Game::FormID>>& a_params,
				const OutfitProfile&                                                           a_profile)
			{
				a_params.data                         = a_profile.Data();
				GetOrCreateEntryData(a_params.params) = a_profile.Data();

				QueueListUpdateCurrent();

				m_controller.QueueEvaluateNPC(
					a_params.params.handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kWantGroupUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
			}

			const ImVec4* UIOutfitEditorNPC::HighlightEntry(Game::FormID a_handle)
			{
				const auto& data = m_controller.GetActiveConfig().outfit.GetNPCData();

				if (auto it = data.find(a_handle); it != data.end())
				{
					return std::addressof(UICommon::g_colorLightOrange);
				}

				return nullptr;
			}

		}
	}
}