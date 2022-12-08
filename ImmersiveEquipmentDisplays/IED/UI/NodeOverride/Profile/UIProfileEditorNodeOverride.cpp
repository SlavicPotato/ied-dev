#include "pch.h"

#include "UIProfileEditorNodeOverride.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIProfileEditorNodeOverride::UIProfileEditorNodeOverride(Controller& a_controller) :
			UIProfileEditorBase<NodeOverrideProfile>(
				UIProfileStrings::TitleNodeOverride,
				"ied_pe_no"),
			UINodeOverrideEditorWidget<UIGlobalEditorDummyHandle>(a_controller),
			UISettingsInterface(a_controller),
			m_controller(a_controller)
		{
		}

		UIProfileEditorNodeOverride::~UIProfileEditorNodeOverride() noexcept
		{
			GetProfileManager().RemoveSink(this);
		}

		void UIProfileEditorNodeOverride::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.transformProfileEditor.sex, false);
		}

		void UIProfileEditorNodeOverride::DrawItem(NodeOverrideProfile& a_profile)
		{
			if (m_cachedItem)
			{
				ImGui::Spacing();
				DrawNodeOverrideEditor(0, m_cachedItem->data);
			}
		}

		constexpr Data::ConfigClass UIProfileEditorNodeOverride::GetConfigClass() const
		{
			return Data::ConfigClass::Global;
		}

		ProfileManager<NodeOverrideProfile>& UIProfileEditorNodeOverride::GetProfileManager() const
		{
			return GlobalProfileManager::GetSingleton<NodeOverrideProfile>();
		}

		UIPopupQueue& UIProfileEditorNodeOverride::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		auto UIProfileEditorNodeOverride::GetCurrentData()
			-> NodeOverrideEditorCurrentData
		{
			if (!m_state.selected ||
			    !m_cachedItem)
			{
				return {};
			}
			else
			{
				return { 0, std::addressof(m_cachedItem->data) };
			}
		}

		void UIProfileEditorNodeOverride::OnItemSelected(
			const stl::fixed_string& a_name)
		{
			const auto& data = GetProfileManager().Data();

			if (auto it = data.find(a_name); it != data.end())
			{
				m_cachedItem.emplace(
					a_name,
					it->second.Data(),
					Data::ConfigClass::Global);
			}
			else
			{
				m_cachedItem.reset();
			}
		}

		void UIProfileEditorNodeOverride::OnProfileRename(
			const stl::fixed_string& a_oldName,
			const stl::fixed_string& a_newName)
		{
			if (!m_cachedItem)
			{
				return;
			}

			if (m_cachedItem->name == a_oldName)
			{
				m_cachedItem->name = a_newName;
			}
		}

		void UIProfileEditorNodeOverride::OnProfileDelete(
			const stl::fixed_string& a_name)
		{
			if (!m_cachedItem)
			{
				return;
			}

			if (m_cachedItem->name == a_name)
			{
				m_cachedItem.reset();
			}
		}

		void UIProfileEditorNodeOverride::OnProfileSave(
			const stl::fixed_string& a_name,
			NodeOverrideProfile&     a_profile)
		{
			if (!m_cachedItem)
			{
				return;
			}

			if (m_cachedItem->name == a_name)
			{
				m_cachedItem.emplace(
					a_name,
					a_profile.Data(),
					Data::ConfigClass::Global);
			}
		}

		void UIProfileEditorNodeOverride::OnProfileReload(
			const NodeOverrideProfile& a_profile)
		{
			if (!m_cachedItem)
			{
				return;
			}

			if (m_cachedItem->name == a_profile.Name())
			{
				m_cachedItem->data = {
					a_profile.Data(),
					Data::ConfigClass::Global
				};
			}
		}

		void UIProfileEditorNodeOverride::OnSexChanged(
			Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.transformProfileEditor.sex != a_newSex)
			{
				store.settings.set(
					store.settings.data.ui.transformProfileEditor.sex,
					a_newSex);
			}
		}

		Data::SettingHolder::EditorPanelCommon& UIProfileEditorNodeOverride::GetEditorPanelSettings()
		{
			return m_controller
			    .GetConfigStore()
			    .settings.data.ui.transformProfileEditor;
		}

		void UIProfileEditorNodeOverride::OnEditorPanelSettingsChange()
		{
			m_controller
				.GetConfigStore()
				.settings.mark_dirty();
		}

		UIData::UICollapsibleStates& UIProfileEditorNodeOverride::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetConfigStore().settings;

			return settings.data.ui.transformProfileEditor.colStates;
		}

		void UIProfileEditorNodeOverride::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.mark_dirty();
		}

		void UIProfileEditorNodeOverride::OnUpdate(
			UIGlobalEditorDummyHandle                      a_handle,
			const SingleNodeOverrideTransformUpdateParams& a_params)
		{
			UpdateConfigSingle(a_params, GetEditorPanelSettings().sexSync);
		}

		void UIProfileEditorNodeOverride::OnUpdate(
			UIGlobalEditorDummyHandle                      a_handle,
			const SingleNodeOverridePlacementUpdateParams& a_params)
		{
			UpdateConfigSingle(a_params, GetEditorPanelSettings().sexSync);
		}

		void UIProfileEditorNodeOverride::OnUpdate(
			UIGlobalEditorDummyHandle                    a_handle,
			const SingleNodeOverridePhysicsUpdateParams& a_params)
		{
			UpdateConfigSingle(a_params, GetEditorPanelSettings().sexSync);
		}

		void UIProfileEditorNodeOverride::OnUpdate(
			UIGlobalEditorDummyHandle       a_handle,
			const NodeOverrideUpdateParams& a_params)
		{
			UpdateConfig(a_params);
		}

		void UIProfileEditorNodeOverride::OnClearTransform(
			UIGlobalEditorDummyHandle            a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			if (auto profile = GetCurrentProfile())
			{
				profile->Data().transformData.erase(a_params.name);
			}
		}

		void UIProfileEditorNodeOverride::OnClearPlacement(
			UIGlobalEditorDummyHandle            a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			if (auto profile = GetCurrentProfile())
			{
				profile->Data().placementData.erase(a_params.name);
			}
		}

		void UIProfileEditorNodeOverride::OnClearPhysics(
			UIGlobalEditorDummyHandle            a_handle,
			const ClearNodeOverrideUpdateParams& a_params)
		{
			if (auto profile = GetCurrentProfile())
			{
				profile->Data().physicsData.erase(a_params.name);
			}
		}

		void UIProfileEditorNodeOverride::OnClearAllTransforms(
			UIGlobalEditorDummyHandle               a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			if (auto profile = GetCurrentProfile())
			{
				profile->Data().transformData.clear();
			}
		}

		void UIProfileEditorNodeOverride::OnClearAllPlacement(
			UIGlobalEditorDummyHandle               a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			if (auto profile = GetCurrentProfile())
			{
				profile->Data().placementData.clear();
			}
		}

		void UIProfileEditorNodeOverride::OnClearAllPhysics(
			UIGlobalEditorDummyHandle               a_handle,
			const ClearAllNodeOverrideUpdateParams& a_params)
		{
			if (auto profile = GetCurrentProfile())
			{
				profile->Data().physicsData.clear();
			}
		}

		WindowLayoutData UIProfileEditorNodeOverride::GetWindowDimensions() const
		{
			return { 250.0f, 600.0f, -1.0f, false };
		}

		UIPopupQueue& UIProfileEditorNodeOverride::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		void UIProfileEditorNodeOverride::UpdateConfig(
			const NodeOverrideUpdateParams& a_params)
		{
			auto profile = GetCurrentProfile();
			if (profile)
			{
				profile->Data() = a_params.data;
			}
		}

		NodeOverrideProfile* UIProfileEditorNodeOverride::GetCurrentProfile() const
		{
			if (m_cachedItem)
			{
				auto& data = GetProfileManager().Data();

				if (auto it = data.find(m_cachedItem->name); it != data.end())
				{
					return std::addressof(it->second);
				}
			}

			return nullptr;
		}

		void UIProfileEditorNodeOverride::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "peb_1")))
			{
				EditorDrawMenuBarItems();
				ImGui::EndMenu();
			}
		}

	}
}