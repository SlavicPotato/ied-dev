#include "pch.h"

#include "UINodeOverrideEditorActor.h"

#include "IED/Controller/Controller.h"
#include "IED/Data.h"

namespace IED
{
	namespace UI
	{
		UINodeOverrideEditorActor::UINodeOverrideEditorActor(Controller& a_controller) :
			UINodeOverrideEditorCommon<Game::FormID>(a_controller),
			UIActorList<entryNodeOverrideData_t>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UITransformSliderWidget(a_controller),
			m_controller(a_controller)
		{
		}

		void UINodeOverrideEditorActor::EditorInitialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformEditor.actorConfig.sex, false);
		}

		void UINodeOverrideEditorActor::EditorDraw()
		{
			if (ImGui::BeginChild("no_editor_actor", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();
				ListDraw();
				ImGui::Separator();
				ImGui::Spacing();

				if (m_listCurrent)
				{
					auto disabled = m_listCurrent->handle == Data::IData::GetPlayerRefID() &&
					                !m_controller.GetNodeOverridePlayerEnabled();

					if (disabled)
					{
						DrawPlayerDisabledWarning();
					}
					else
					{
						if (!m_controller.SkeletonCheck(m_listCurrent->handle))
						{
							ImGui::TextColored(
								UICommon::g_colorWarning,
								"%s",
								LS(UIWidgetCommonStrings::XP32SkeletonWarning));

							ImGui::Spacing();
							ImGui::Separator();
						}
					}

					UICommon::PushDisabled(disabled);

					DrawNodeOverrideEditor(m_listCurrent->handle, m_listCurrent->data);

					UICommon::PopDisabled(disabled);
				}
			}

			ImGui::EndChild();
		}

		void UINodeOverrideEditorActor::EditorOnOpen()
		{
			EditorReset();
		}

		void UINodeOverrideEditorActor::EditorOnClose()
		{
			EditorReset();
		}

		void UINodeOverrideEditorActor::EditorReset()
		{
			ListReset();
		}

		constexpr Data::ConfigClass UINodeOverrideEditorActor::GetConfigClass() const
		{
			return Data::ConfigClass::Actor;
		}

		Data::SettingHolder::EditorPanelActorSettings& UINodeOverrideEditorActor::GetActorSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor.actorConfig;
		}

		auto UINodeOverrideEditorActor::GetCurrentData()
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

		UIData::UICollapsibleStates& UINodeOverrideEditorActor::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			return settings.data.ui.transformEditor
			    .colStates[stl::underlying(Data::ConfigClass::Actor)];
		}

		void UINodeOverrideEditorActor::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		Data::SettingHolder::EditorPanelCommon& UINodeOverrideEditorActor::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor;
		}

		void UINodeOverrideEditorActor::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UINodeOverrideEditorActor::OnListOptionsChange()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UINodeOverrideEditorActor::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UINodeOverrideEditorActor::OnListChangeCurrentItem(
			const stl::optional<UIActorList<entryNodeOverrideData_t>::listValue_t>& a_oldHandle,
			const stl::optional<UIActorList<entryNodeOverrideData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& settings = m_controller.GetConfigStore().settings;

			if (!settings.data.ui.transformEditor.actorConfig.autoSelectSex)
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

		entryNodeOverrideData_t UINodeOverrideEditorActor::GetData(Game::FormID a_handle)
		{
			auto& actorInfo = m_controller.GetActorInfo();

			if (auto it = actorInfo.find(a_handle); it != actorInfo.end())
			{
				auto& store = m_controller.GetConfigStore();

				return store.active.transforms.GetActorCopy(
					a_handle,
					it->second.GetBaseOrTemplate(),
					it->second.GetRace());
			}
			else
			{
				return {};
			}
		}

		void UINodeOverrideEditorActor::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.transformEditor.actorConfig.sex != a_newSex)
			{
				store.settings.set(
					store.settings.data.ui.transformEditor.actorConfig.sex,
					a_newSex);
			}
		}

		void UINodeOverrideEditorActor::ApplyProfile(
			const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile&                               a_profile)
		{
			GetOrCreateConfigHolder(a_data.handle) = a_profile.Data();

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransformsActor(a_data.handle, true);
		}

		void UINodeOverrideEditorActor::MergeProfile(
			const profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile&                               a_profile)
		{
			MergeProfileData(a_data, a_profile);

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransformsActor(a_data.handle, true);
		}

		void UINodeOverrideEditorActor::OnUpdate(
			Game::FormID                                   a_handle,
			const SingleNodeOverrideTransformUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsActor(a_handle, true);
		}

		void UINodeOverrideEditorActor::OnUpdate(
			Game::FormID                                   a_handle,
			const SingleNodeOverridePlacementUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsActor(a_handle, true);
		}

		void UINodeOverrideEditorActor::OnUpdate(
			Game::FormID                                 a_handle,
			const SingleNodeOverridePhysicsUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfigSingle(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransformsActor(a_handle, true);
		}

		void UINodeOverrideEditorActor::OnUpdate(
			Game::FormID                    a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			UpdateConfig(a_handle, a_params);

			a_params.data = GetData(a_handle);

			m_controller.RequestEvaluateTransformsActor(a_handle, true);
		}

		void UINodeOverrideEditorActor::OnClearTransform(
			Game::FormID                         a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller
			                 .GetConfigStore()
			                 .active.transforms.GetActorData();

			if (EraseConfig<
					Data::configNodeOverrideEntryTransform_t>(
					a_handle,
					data,
					a_params.name))
			{
				m_controller.RequestEvaluateTransformsActor(a_handle, true);
			}

			PostClear(
				GetData(a_handle).transformData,
				a_params.entry.transformData,
				a_params.name);
		}

		void UINodeOverrideEditorActor::OnClearPlacement(
			Game::FormID                         a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller
			                 .GetConfigStore()
			                 .active.transforms.GetActorData();

			if (EraseConfig<
					Data::configNodeOverrideEntryPlacement_t>(
					a_handle,
					data,
					a_params.name))
			{
				m_controller.RequestEvaluateTransformsActor(a_handle, true);
			}

			PostClear(
				GetData(a_handle).placementData,
				a_params.entry.placementData,
				a_params.name);
		}

		void UINodeOverrideEditorActor::OnClearPhysics(
			Game::FormID                         a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller
			                 .GetConfigStore()
			                 .active.transforms.GetActorData();

			if (EraseConfig<
					Data::configNodeOverrideEntryPhysics_t>(
					a_handle,
					data,
					a_params.name))
			{
				m_controller.RequestEvaluateTransformsActor(a_handle, true);
			}

			PostClear(
				GetData(a_handle).physicsData,
				a_params.entry.physicsData,
				a_params.name);
		}

		void UINodeOverrideEditorActor::OnClearAllTransforms(
			Game::FormID                            a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetActorData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				it->second.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);
				it->second.transformData.clear();

				m_controller.RequestEvaluateTransformsActor(a_handle, true);
			}

			a_params.entry.transformData = GetData(a_handle).transformData;
		}

		void UINodeOverrideEditorActor::OnClearAllPlacement(
			Game::FormID                            a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetActorData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				it->second.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);
				it->second.placementData.clear();

				m_controller.RequestEvaluateTransformsActor(a_handle, true);
			}

			a_params.entry.placementData = GetData(a_handle).placementData;
		}

		void UINodeOverrideEditorActor::OnClearAllPhysics(
			Game::FormID a_handle, const ClearAllNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetActorData();

			auto it = data.find(a_handle);
			if (it != data.end())
			{
				it->second.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);
				it->second.physicsData.clear();

				m_controller.RequestEvaluateTransformsActor(a_handle, true);
			}

			a_params.entry.physicsData = GetData(a_handle).physicsData;
		}

		Data::configNodeOverrideHolder_t& UINodeOverrideEditorActor::GetOrCreateConfigHolder(
			Game::FormID a_handle) const
		{
			auto& result = m_controller
			                   .GetConfigStore()
			                   .active.transforms.GetActorData()
			                   .try_emplace(a_handle)
			                   .first->second;

			result.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);

			return result;
		}

		Data::configNodeOverrideHolder_t* UINodeOverrideEditorActor::GetConfigHolder(Game::FormID a_handle) const
		{
			auto& data = m_controller
			                 .GetConfigStore()
			                 .active.transforms.GetActorData();

			auto it = data.find(a_handle);

			return it != data.end() ? std::addressof(it->second) : nullptr;
		}

		UIPopupQueue& UINodeOverrideEditorActor::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UINodeOverrideEditorActor::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		const ImVec4* UINodeOverrideEditorActor::HighlightEntry(Game::FormID a_handle)
		{
			const auto& data = m_controller
			                       .GetConfigStore()
			                       .active.transforms.GetActorData();

			if (auto it = data.find(a_handle); it != data.end() && !it->second.empty())
			{
				return std::addressof(UICommon::g_colorLimeGreen);
			}

			return nullptr;
		}

	}
}