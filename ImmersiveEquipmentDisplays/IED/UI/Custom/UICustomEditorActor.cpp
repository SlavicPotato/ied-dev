#include "pch.h"

#include "IED/UI/UIFormInfoCache.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "UICustomEditorActor.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UICustomEditorActor::UICustomEditorActor(
			Controller& a_controller) :
			UICustomEditorCommon<Game::FormID>(a_controller),
			UIActorList<entryCustomData_t>(a_controller),
			UIFormInfoTooltipWidget(a_controller),
			UIMiscTextInterface(a_controller),
			UISettingsInterface(a_controller),
			UIActorInfoInterface(a_controller),
			UIPopupInterface(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			m_controller(a_controller)
		{
		}

		UICustomEditorActor::~UICustomEditorActor() noexcept
		{
			GlobalProfileManager::GetSingleton<CustomProfile>().RemoveSink(this);
		}

		void UICustomEditorActor::EditorInitialize()
		{
			InitializeProfileBase();

			const auto& store = m_controller.GetSettings();

			SetSex(store.data.ui.customEditor.actorConfig.sex, false);
		}

		void UICustomEditorActor::EditorDraw()
		{
			if (ImGui::BeginChild("custom_editor_actor", { -1.0f, 0.0f }))
			{
				ImGui::Spacing();
				ListDraw();
				ImGui::Separator();
				ImGui::Spacing();

				if (m_listCurrent)
				{
					if (m_controller.IsActorBlockedImpl(m_listCurrent->handle))
					{
						ImGui::TextColored(
							UICommon::g_colorWarning,
							"%s",
							UIL::LS(UIWidgetCommonStrings::ActorBlocked));

						ImGui::Spacing();
					}

					/*if (!m_controller.SkeletonCheck(m_listCurrent->handle))
					{
						ImGui::TextColored(
							UICommon::g_colorWarning,
							"%s",
							UIL::LS(UIWidgetCommonStrings::XP32SkeletonWarning));

						ImGui::Spacing();
					}*/

					DrawCustomEditor(m_listCurrent->handle, m_listCurrent->data);
				}
			}

			ImGui::EndChild();
		}

		void UICustomEditorActor::EditorOnOpen()
		{
			EditorReset();
		}

		void UICustomEditorActor::EditorOnClose()
		{
			EditorReset();
		}

		void UICustomEditorActor::EditorReset()
		{
			ListReset();
		}

		void UICustomEditorActor::EditorQueueUpdateCurrent()
		{
			QueueListUpdateCurrent();
		}

		constexpr Data::ConfigClass UICustomEditorActor::GetConfigClass() const
		{
			return Data::ConfigClass::Actor;
		}

		Data::SettingHolder::EditorPanelActorSettings& UICustomEditorActor::GetActorSettings() const
		{
			return m_controller.GetSettings().data.ui.customEditor.actorConfig;
		}

		Data::configCustomHolder_t& UICustomEditorActor::GetOrCreateConfigSlotHolder(
			Game::FormID a_handle) const
		{
			auto& data = m_controller.GetActiveConfig().custom.GetActorData();
			auto& sh   = StringHolder::GetSingleton();

			auto& pluginMap = data.try_emplace(a_handle).first->second;

			return pluginMap.try_emplace(sh.IED).first->second;
		}

		entryCustomData_t UICustomEditorActor::GetData(Game::FormID a_handle)
		{
			auto& store = m_controller.GetActiveConfig();
			auto& data  = store.custom.GetActorData();

			auto it = data.find(static_cast<Data::configForm_t>(a_handle));
			if (it != data.end())
			{
				auto& sh = StringHolder::GetSingleton();

				auto it2 = it->second.find(sh.IED);
				if (it2 != it->second.end())
				{
					return it2->second;
				}
			}

			return {};
		}

		auto UICustomEditorActor::GetCurrentData()
			-> CustomEditorCurrentData
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

		UIPopupQueue& UICustomEditorActor::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		UIPopupQueue& UICustomEditorActor::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		UIData::UICollapsibleStates& UICustomEditorActor::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetSettings();

			return settings.data.ui.customEditor
			    .colStates[stl::underlying(Data::ConfigClass::Actor)];
		}

		void UICustomEditorActor::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UICustomEditorActor::OnListOptionsChange()
		{
			GetSettings().mark_dirty();
		}

		Data::SettingHolder::EditorPanelCommon& UICustomEditorActor::GetEditorPanelSettings()
		{
			return m_controller.GetSettings().data.ui.customEditor;
		}

		void UICustomEditorActor::OnEditorPanelSettingsChange()
		{
			GetSettings().mark_dirty();
		}

		void UICustomEditorActor::ListResetAllValues(Game::FormID a_handle)
		{
		}

		void UICustomEditorActor::OnListChangeCurrentItem(
			const std::optional<UIActorList<entryCustomData_t>::listValue_t>& a_oldHandle,
			const std::optional<UIActorList<entryCustomData_t>::listValue_t>& a_newHandle)
		{
			if (!a_newHandle)
			{
				return;
			}

			auto& settings = GetSettings();

			if (!settings.data.ui.customEditor.actorConfig.autoSelectSex)
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

		void UICustomEditorActor::OnSexChanged(Data::ConfigSex a_newSex)
		{
			auto& settings = GetSettings();

			if (settings.data.ui.customEditor.actorConfig.sex != a_newSex)
			{
				settings.set(
					settings.data.ui.customEditor.actorConfig.sex,
					a_newSex);
			}
		}

		void UICustomEditorActor::ApplyProfile(
			const profileSelectorParamsCustom_t<Game::FormID>& a_data,
			const CustomProfile&                               a_profile)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_data.handle);

			a_data.data = a_profile.Data();
			conf        = a_profile.Data();

			m_controller.QueueResetCustom(
				a_data.handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorActor::MergeProfile(
			const profileSelectorParamsCustom_t<Game::FormID>& a_data,
			const CustomProfile&                               a_profile)
		{
			auto& profileData = a_profile.Data();

			for (auto& [i, e] : profileData.data)
			{
				a_data.data.data.insert_or_assign(i, e);
			}

			GetOrCreateConfigSlotHolder(a_data.handle) = a_data.data;

			m_controller.QueueResetCustom(
				a_data.handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		void UICustomEditorActor::OnBaseConfigChange(
			Game::FormID     a_handle,
			const void*      a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleCustomConfigUpdateParams*>(a_params);

			UpdateConfig(a_handle, *params, GetSettings().data.ui.customEditor.sexSync);

			switch (a_action)
			{
			case PostChangeAction::Evaluate:
			case PostChangeAction::UpdateTransform:
				m_controller.QueueEvaluate(
					a_handle,
					ControllerUpdateFlags::kWantEffectShaderConfigUpdate |
						ControllerUpdateFlags::kWantGroupUpdate |
						ControllerUpdateFlags::kImmediateTransformUpdate);
				break;
			case PostChangeAction::Reset:
				m_controller.QueueResetCustom(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					params->name);
				break;
			}
		}

		void UICustomEditorActor::OnFullConfigChange(
			Game::FormID                    a_handle,
			const CustomConfigUpdateParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			conf = a_params.data;

			//m_controller.QueueActorReset(a_handle, ControllerUpdateFlags::kNone);
			m_controller.QueueResetCustom(
				a_handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED);
		}

		bool UICustomEditorActor::OnCreateNew(
			Game::FormID                 a_handle,
			const CustomConfigNewParams& a_params)
		{
			auto& conf = GetOrCreateConfigSlotHolder(a_handle);

			auto r = conf.data.try_emplace(a_params.name, a_params.entry).second;

			if (r)
			{
				m_controller.QueueEvaluate(a_handle, ControllerUpdateFlags::kNone);
			}

			return r;
		}

		void UICustomEditorActor::OnErase(
			Game::FormID                   a_handle,
			const CustomConfigEraseParams& a_params)
		{
			auto& data = m_controller.GetActiveConfig().custom.GetActorData();

			if (EraseConfig(a_handle, data, a_params.name))
			{
				m_controller.QueueResetCustom(
					a_handle,
					GetConfigClass(),
					StringHolder::GetSingleton().IED,
					a_params.name);
			}
		}

		bool UICustomEditorActor::OnRename(
			Game::FormID                    a_handle,
			const CustomConfigRenameParams& a_params)
		{
			if (!DoConfigRename(a_handle, a_params))
			{
				return false;
			}

			m_controller.QueueResetCustom(
				a_handle,
				GetConfigClass(),
				StringHolder::GetSingleton().IED,
				a_params.oldName);

			return true;
		}

		void UICustomEditorActor::DrawMenuBarItemsExtra()
		{
			auto& entry = ListGetSelected();
			if (!entry)
			{
				return;
			}

			ImGui::Separator();

			if (ImGui::BeginMenu(entry->desc.c_str()))
			{
				if (ImGui::MenuItem("Evaluate"))
				{
					m_controller.QueueEvaluate(entry->handle, ControllerUpdateFlags::kWantEffectShaderConfigUpdate);
				}

				if (ImGui::MenuItem("Reset"))
				{
					m_controller.QueueReset(entry->handle, ControllerUpdateFlags::kNone);
				}

				ImGui::Separator();

				if (ImGui::MenuItem("NiNode Update"))
				{
					m_controller.QueueNiNodeUpdate(entry->handle);
				}

				ImGui::EndMenu();
			}
		}

		auto UICustomEditorActor::GetLoadedObject(
			Game::FormID                     a_handle,
			const stl::fixed_string&         a_name,
			const Data::configCustomEntry_t& a_entry)
			-> const ObjectEntryCustom*
		{
			auto& data = m_controller.GetActorMap();

			auto it = data.find(a_handle);
			if (it == data.end())
			{
				return nullptr;
			}

			auto& cust = it->second.GetCustom(Data::ConfigClass::Actor);

			auto& sh = StringHolder::GetSingleton();

			auto it2 = cust.find(sh.IED);
			if (it2 == cust.end())
			{
				return nullptr;
			}

			auto it3 = it2->second.find(a_name);
			if (it3 == it2->second.end())
			{
				return nullptr;
			}

			if (!it3->second.data.state)
			{
				return nullptr;
			}

			return std::addressof(it3->second);
		}

		const ImVec4* UICustomEditorActor::HighlightEntry(Game::FormID a_handle)
		{
			return HasConfigEntry(
					   m_controller.GetActiveConfig().custom.GetActorData(),
					   a_handle) ?
			           std::addressof(UICommon::g_colorLimeGreen) :
                       nullptr;
		}

		bool UICustomEditorActor::DrawExtraItemInfo(
			Game::FormID                     a_handle,
			const stl::fixed_string&         a_name,
			const Data::configCustomEntry_t& a_entry,
			bool                             a_infoDrawn)
		{
			ImGui::TextUnformatted("Item:");
			ImGui::SameLine();

			auto object = GetLoadedObject(a_handle, a_name, a_entry);
			if (!object)
			{
				ImGui::TextColored(UICommon::g_colorGreyed, "%s", "Not loaded");
				return true;
			}

			auto& flc = m_controller.UIGetFormLookupCache();
			DrawObjectEntryHeaderInfo(flc.LookupForm(object->data.state->form->formID), *object);

			return true;
		}
	}
}
