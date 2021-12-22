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
			m_controller(a_controller)
		{
		}

		void UINodeOverrideEditorActor::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformEditor.actorConfig.sex, false);
		}

		void UINodeOverrideEditorActor::Draw()
		{
			if (ImGui::BeginChild("transform_editor_actor", { -1.0f, 0.0f }))
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
					auto disabled = (entry->handle == Data::IData::GetPlayerRefID() && !m_controller.GetNodeOverridePlayerEnabled());

					if (disabled)
					{
						DrawPlayerDisabledWarning();
					}

					UICommon::PushDisabled(disabled);

					DrawNodeOverrideEditor(entry->handle, entry->data);

					UICommon::PopDisabled(disabled);
				}
			}

			ImGui::EndChild();
		}

		void UINodeOverrideEditorActor::OnOpen()
		{
			Reset();
		}

		void UINodeOverrideEditorActor::OnClose()
		{
			Reset();
		}

		void UINodeOverrideEditorActor::Reset()
		{
			ListReset();
		}

		constexpr Data::ConfigClass UINodeOverrideEditorActor::GetConfigClass() const
		{
			return Data::ConfigClass::Actor;
		}

		const ActorInfoHolder& UINodeOverrideEditorActor::GetActorInfoHolder() const
		{
			return m_controller.GetActorInfo();
		}

		std::uint64_t UINodeOverrideEditorActor::GetActorInfoUpdateID() const
		{
			return m_controller.GetActorInfoUpdateID();
		}

		Data::SettingHolder::EditorPanelActorSettings& UINodeOverrideEditorActor::GetActorSettings() const
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor.actorConfig;
		}

		const SetObjectWrapper<Game::FormID>& UINodeOverrideEditorActor::GetCrosshairRef()
		{
			return m_controller.GetCrosshairRef();
		}

		auto UINodeOverrideEditorActor::GetCurrentData()
			-> NodeOverrideEditorCurrentData
		{
			if (auto entry = ListGetSelected())
			{
				return { entry->handle, std::addressof(entry->data) };
			}
			else
			{
				return { {}, nullptr };
			}
		}

		UIData::UICollapsibleStates& UINodeOverrideEditorActor::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.transformEditor
			    .colStates[stl::underlying(Data::ConfigClass::Actor)];
		}

		void UINodeOverrideEditorActor::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		Data::SettingHolder::EditorPanelCommon& UINodeOverrideEditorActor::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.transformEditor;
		}

		void UINodeOverrideEditorActor::OnEditorPanelSettingsChange()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UINodeOverrideEditorActor::OnListOptionsChange()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UINodeOverrideEditorActor::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UINodeOverrideEditorActor::OnListChangeCurrentItem(
			const SetObjectWrapper<UIActorList<entryNodeOverrideData_t>::listValue_t>& a_oldHandle,
			const SetObjectWrapper<UIActorList<entryNodeOverrideData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& config = m_controller.GetConfigStore().settings;

			if (!config.data.ui.transformEditor.actorConfig.autoSelectSex)
			{
				return;
			}

			auto& actorInfo = m_controller.GetActorInfo();

			auto it = actorInfo.find(a_newHandle->handle);
			if (it != actorInfo.end())
			{
				auto sex = it->second.GetSex();

				SetSex(sex, false);
			}
		}

		const entryNodeOverrideData_t& UINodeOverrideEditorActor::GetData(Game::FormID a_handle)
		{
			auto& actorInfo = m_controller.GetActorInfo();

			if (auto it = actorInfo.find(a_handle); it != actorInfo.end())
			{
				auto& store = m_controller.GetConfigStore();

				m_temp = store.active.transforms.GetActor(
					a_handle,
					it->second.GetBase(),
					it->second.GetRace());
			}
			else
			{
				m_temp.clear();
			}

			return m_temp;
		}

		const NodeOverrideProfile::base_type& UINodeOverrideEditorActor::GetData(
			const profileSelectorParamsNodeOverride_t<Game::FormID>& a_params)
		{
			auto& store = m_controller.GetConfigStore();
			auto& data = store.active.transforms.GetActorData();

			if (auto it = data.find(a_params.handle); it != data.end())
			{
				m_temp = it->second;
			}
			else
			{
				m_temp.clear();
			}

			return m_temp;
		}

		void UINodeOverrideEditorActor::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			store.settings.Set(
				store.settings.data.ui.transformEditor.actorConfig.sex,
				a_newSex);
		}

		void UINodeOverrideEditorActor::ApplyProfile(
			profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			GetOrCreateConfigHolder(a_data.handle) = a_profile.Data();

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransforms(a_data.handle, true);
		}

		void UINodeOverrideEditorActor::MergeProfile(
			profileSelectorParamsNodeOverride_t<Game::FormID>& a_data,
			const NodeOverrideProfile& a_profile)
		{
			auto& conf = GetOrCreateConfigHolder(a_data.handle);

			for (auto& e : a_profile.Data().data)
			{
				conf.data.insert_or_assign(e.first, e.second);
			}

			a_data.data = GetData(a_data.handle);

			m_controller.RequestEvaluateTransforms(a_data.handle, true);
		}

		void UINodeOverrideEditorActor::OnUpdate(
			Game::FormID a_handle,
			const SingleNodeOverrideUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfig(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransforms(a_handle, true);
		}

		void UINodeOverrideEditorActor::OnUpdate(
			Game::FormID a_handle,
			const SingleNodeOverrideParentUpdateParams& a_params)
		{
			auto& store = m_controller.GetConfigStore();

			UpdateConfig(a_handle, a_params, store.settings.data.ui.transformEditor.sexSync);

			m_controller.RequestEvaluateTransforms(a_handle, true);
		}

		/*void UINodeOverrideEditorActor::OnUpdate(
			Game::FormID a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			auto& conf = GetOrCreateConfigHolder(a_handle);

			conf = a_params.data;

			m_controller.RequestEvaluateTransforms(a_handle, true);
		}*/

		void UINodeOverrideEditorActor::OnClear(
			Game::FormID a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetActorData();

			if (EraseConfig(a_handle, data, a_params.name))
			{
				PostClear(
					GetData(a_handle).data,
					a_params.entry.data,
					a_params.name);

				m_controller.RequestEvaluateTransforms(a_handle, true);
			}
		}

		void UINodeOverrideEditorActor::OnClearParent(
			Game::FormID a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetActorData();

			if (EraseConfigParent(a_handle, data, a_params.name))
			{
				PostClear(
					GetData(a_handle).placementData,
					a_params.entry.placementData,
					a_params.name);

				m_controller.RequestEvaluateTransforms(a_handle, true);
			}
		}

		Data::configNodeOverrideHolder_t& UINodeOverrideEditorActor::GetOrCreateConfigHolder(Game::FormID a_handle) const
		{
			auto& data = m_controller.GetConfigStore().active.transforms.GetActorData();

			return data.try_emplace(a_handle).first->second;
		}

		UIPopupQueue& UINodeOverrideEditorActor::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

	}
}