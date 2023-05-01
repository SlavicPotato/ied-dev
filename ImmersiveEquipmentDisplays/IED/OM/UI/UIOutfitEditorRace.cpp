#include "pch.h"

#include "UIOutfitEditorRace.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		namespace OM
		{
			UIOutfitEditorRace::UIOutfitEditorRace(
				Controller& a_controller) :
				UIOutfitEntryEditorCommon<
					UIOutfitEditorParamsHandle<Game::FormID>>(a_controller),
				UIRaceList<UIOutfitEntryListItemWrapper>(a_controller),
				UISettingsInterface(a_controller),
				UIActorInfoInterface(a_controller),
				UIEquipmentOverrideConditionsWidget(a_controller),
				m_controller(a_controller)
			{
			}

			UIOutfitEditorRace::~UIOutfitEditorRace() noexcept
			{
				GlobalProfileManager::GetSingleton<OutfitProfile>().RemoveSink(this);
			}

			void UIOutfitEditorRace::EditorInitialize()
			{
				InitializeProfileBase();

				const auto& settings = m_controller.GetSettings();

				SetSex(settings.data.ui.outfitEditor.raceConfig.sex, false);
			}

			void UIOutfitEditorRace::EditorDraw()
			{
				if (ImGui::BeginChild("otft_editor_race", { -1.0f, 0.0f }))
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

			void UIOutfitEditorRace::EditorOnOpen()
			{
				EditorReset();
			}

			void UIOutfitEditorRace::EditorOnClose()
			{
				EditorReset();
			}

			void UIOutfitEditorRace::EditorReset()
			{
				ListReset();
			}

			void UIOutfitEditorRace::EditorQueueUpdateCurrent()
			{
				QueueListUpdateCurrent();
			}

			constexpr Data::ConfigClass UIOutfitEditorRace::GetConfigClass() const
			{
				return Data::ConfigClass::Race;
			}

			Data::SettingHolder::EditorPanelRaceSettings& UIOutfitEditorRace::GetRaceSettings() const
			{
				return m_controller.GetSettings().data.ui.outfitEditor.raceConfig;
			}

			Data::SettingHolder::EditorPanelCommon& UIOutfitEditorRace::GetEditorPanelSettings()
			{
				return m_controller.GetSettings().data.ui.outfitEditor;
			}

			void UIOutfitEditorRace::OnEditorPanelSettingsChange()
			{
				m_controller.GetSettings().mark_dirty();
			}

			UIOutfitEntryListItemWrapper UIOutfitEditorRace::GetData(Game::FormID a_handle)
			{
				const auto& store = m_controller.GetActiveConfig().outfit;

				const auto& data = store.GetRaceData();

				auto it2 = data.find(a_handle);
				if (it2 != data.end())
				{
					return it2->second;
				}
				else
				{
					return {
						UIOutfitEntryListItemWrapper::nodata_arg_t{},
						store.GetRace(a_handle)
					};
				}
			}

			void UIOutfitEditorRace::OnListChangeCurrentItem(
				const std::optional<UIRaceList<UIOutfitEntryListItemWrapper>::listValue_t>& a_oldHandle,
				const std::optional<UIRaceList<UIOutfitEntryListItemWrapper>::listValue_t>& a_newHandle)
			{
			}

			void UIOutfitEditorRace::OnSexChanged(Data::ConfigSex a_newSex)
			{
				auto& settings = m_controller.GetSettings();

				if (settings.data.ui.outfitEditor.raceConfig.sex != a_newSex)
				{
					settings.set(
						settings.data.ui.outfitEditor.raceConfig.sex,
						a_newSex);
				}
			}

			void UIOutfitEditorRace::OnListOptionsChange()
			{
				m_controller.GetSettings().mark_dirty();
			}

			UIPopupQueue& UIOutfitEditorRace::GetPopupQueue_ProfileBase() const
			{
				return m_controller.UIGetPopupQueue();
			}

			UIData::UICollapsibleStates& UIOutfitEditorRace::GetCollapsibleStatesData()
			{
				auto& settings = m_controller.GetSettings();

				return settings.data.ui.outfitEditor
				    .colStates[stl::underlying(Data::ConfigClass::Race)];
			}

			void UIOutfitEditorRace::OnCollapsibleStatesUpdate()
			{
				m_controller.GetSettings().mark_dirty();
			}

			void UIOutfitEditorRace::ListResetAllValues(Game::FormID a_handle)
			{
			}

			entryOutfitData_t& UIOutfitEditorRace::GetOrCreateEntryData(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const
			{
				auto& data = m_controller.GetActiveConfig().outfit.GetRaceData();

				return data.try_emplace(a_params.handle).first->second;
			}

			std::optional<std::reference_wrapper<entryOutfitData_t>> UIOutfitEditorRace::GetEntryData(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params) const
			{
				auto& data = m_controller.GetActiveConfig().outfit.GetRaceData();

				auto it = data.find(a_params.handle);
				if (it != data.end())
				{
					return it->second;
				}

				return {};
			}

			void UIOutfitEditorRace::OnOutfitEntryChange(
				const change_param_type& a_params)
			{
				const auto& settings = m_controller.GetSettings();

				UpdateConfig(a_params, settings.data.ui.outfitEditor.sexSync);

				if (a_params.params.noData)
				{
					QueueListUpdateCurrent();
				}

				m_controller.QueueEvaluateRace(
					a_params.params.actual.handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kWantGroupUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
			}

			void UIOutfitEditorRace::OnOutfitEntryErase(
				const UIOutfitEditorParamsHandle<Game::FormID>& a_params)
			{
				auto& data = m_controller.GetActiveConfig().outfit.GetRaceData();

				if (data.erase(a_params.handle))
				{
					QueueListUpdateCurrent();

					m_controller.QueueEvaluateRace(
						a_params.handle,
						ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
							ControllerUpdateFlags::kWantGroupUpdate |
							ControllerUpdateFlags::kImmediateTransformUpdate);
				}
			}

			void UIOutfitEditorRace::ApplyProfile(
				const profileSelectorParamsOutfit_t<UIOutfitEditorParamsHandle<Game::FormID>>& a_params,
				const OutfitProfile&                                                           a_profile)
			{
				a_params.data                         = a_profile.Data();
				GetOrCreateEntryData(a_params.params) = a_profile.Data();

				QueueListUpdateCurrent();

				m_controller.QueueEvaluateRace(
					a_params.params.handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kWantGroupUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
			}

			const ImVec4* UIOutfitEditorRace::HighlightEntry(Game::FormID a_handle)
			{
				const auto& data = m_controller.GetActiveConfig().outfit.GetRaceData();

				if (auto it = data.find(a_handle); it != data.end())
				{
					return std::addressof(UICommon::g_colorPurple);
				}

				return nullptr;
			}

		}
	}
}