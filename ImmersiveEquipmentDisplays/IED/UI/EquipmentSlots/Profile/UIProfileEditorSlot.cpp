#include "pch.h"

#include "UIProfileEditorSlot.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIProfileEditorSlot::UIProfileEditorSlot(Controller& a_controller) :
			UIProfileEditorBase<SlotProfile>(
				UIProfileStrings::TitleSlot,
				"ied_pe_slot"),
			UISlotEditorBase<UIGlobalEditorDummyHandle>(a_controller),
			UIPopupInterface(a_controller),
			UISettingsInterface(a_controller),
			UIEquipmentOverrideConditionsWidget(a_controller),
			m_controller(a_controller)
		{
		}

		UIProfileEditorSlot::~UIProfileEditorSlot() noexcept
		{
			GetProfileManager().RemoveSink(this);
		}

		void UIProfileEditorSlot::Initialize()
		{
			InitializeProfileBase();

			auto& settings = m_controller.GetSettings();

			SetSex(settings.data.ui.slotProfileEditor.sex, false);
		}

		constexpr Data::ConfigClass UIProfileEditorSlot::GetConfigClass() const
		{
			return Data::ConfigClass::Global;
		}

		ProfileManager<SlotProfile>& UIProfileEditorSlot::GetProfileManager() const
		{
			return GlobalProfileManager::GetSingleton<SlotProfile>();
		}

		UIPopupQueue& UIProfileEditorSlot::GetPopupQueue_ProfileBase() const
		{
			return m_controller.UIGetPopupQueue();
		}

		auto UIProfileEditorSlot::GetCurrentData()
			-> SlotEditorCurrentData
		{
			if (!m_state.selected)
			{
				return {};
			}

			if (!m_cachedItem)
			{
				return {};
			}
			else
			{
				return { 0, std::addressof(m_cachedItem->data) };
			}
		}

		UIPopupQueue& UIProfileEditorSlot::GetPopupQueue()
		{
			return m_controller.UIGetPopupQueue();
		}

		void UIProfileEditorSlot::OnItemSelected(
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

		void UIProfileEditorSlot::OnProfileRename(
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

		void UIProfileEditorSlot::OnProfileDelete(
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

		void UIProfileEditorSlot::OnProfileSave(
			const stl::fixed_string& a_name,
			SlotProfile&             a_profile)
		{
			//_DMESSAGE("save: %s", a_name.c_str());

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

		void UIProfileEditorSlot::OnProfileReload(
			const SlotProfile& a_profile)
		{
			if (!m_cachedItem)
			{
				return;
			}

			if (m_cachedItem->name == a_profile.Name())
			{
				m_cachedItem.emplace(
					a_profile.Name(),
					a_profile.Data(),
					Data::ConfigClass::Global);
			}
		}

		void UIProfileEditorSlot::OnSexChanged(
			Data::ConfigSex a_newSex)
		{
			auto& settings = m_controller.GetSettings();

			if (settings.data.ui.slotProfileEditor.sex != a_newSex)
			{
				settings.set(
					settings.data.ui.slotProfileEditor.sex,
					a_newSex);
			}
		}

		Data::SettingHolder::EditorPanelCommon& UIProfileEditorSlot::GetEditorPanelSettings()
		{
			return m_controller.GetSettings().data.ui.slotProfileEditor;
		}

		void UIProfileEditorSlot::OnEditorPanelSettingsChange()
		{
			m_controller.GetSettings().mark_dirty();
		}

		UIData::UICollapsibleStates& UIProfileEditorSlot::GetCollapsibleStatesData()
		{
			auto& settings = m_controller.GetSettings();

			return settings.data.ui.slotProfileEditor.colStates;
		}

		void UIProfileEditorSlot::OnCollapsibleStatesUpdate()
		{
			m_controller.GetSettings().mark_dirty();
		}

		void UIProfileEditorSlot::OnBaseConfigChange(
			UIGlobalEditorDummyHandle,
			const void*      a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				auto sync = GetEditorPanelSettings().sexSync;
				auto sex  = params->sex;

				auto& src = params->entry.second;
				auto& dst = it->second.Data().get(params->slot);

				if (sync)
				{
					src.get(Data::GetOppositeSex(sex)) = src.get(sex);
				}

				if (!dst)
				{
					dst = std::make_unique<Data::configSlotHolder_t::data_type>(src);
				}
				else
				{
					if (sync)
					{
						*dst = src;
					}
					else
					{
						dst->get(sex) = src.get(sex);
					}
				}
			}
		}

		void UIProfileEditorSlot::OnFullConfigChange(
			UIGlobalEditorDummyHandle,
			const SlotConfigUpdateParams& a_params)
		{
			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				it->second.Data() = a_params.data;
			}
		}

		void UIProfileEditorSlot::OnPriorityConfigChange(
			UIGlobalEditorDummyHandle,
			const SlotPriorityConfigUpdateParams& a_params)
		{
			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				auto sync = GetEditorPanelSettings().sexSync;
				auto sex  = a_params.sex;

				auto& src = a_params.entry.second;
				auto& dst = it->second.Data().priority;

				if (sync)
				{
					src.get(Data::GetOppositeSex(sex)) = src.get(sex);
				}

				if (!dst)
				{
					dst = std::make_unique<Data::configSlotHolder_t::prio_data_type>(src);
				}
				else
				{
					if (sync)
					{
						*dst = src;
					}
					else
					{
						dst->get(sex) = src.get(sex);
					}
				}
			}
		}

		void UIProfileEditorSlot::OnPriorityConfigClear(
			UIGlobalEditorDummyHandle  a_handle,
			const SlotConfigUpdateParams& a_params)
		{
			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				Data::assign_uptr(
					a_params.data.priority,
					it->second.Data().priority);
			}
		}

		void UIProfileEditorSlot::OnSingleSlotClear(
			UIGlobalEditorDummyHandle,
			const SingleSlotConfigClearParams& a_params)
		{
			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				it->second.Data().get(a_params.slot).reset();
			}
		}

		void UIProfileEditorSlot::OnFullConfigClear(
			UIGlobalEditorDummyHandle     a_handle,
			const FullSlotConfigClearParams& a_params)
		{
			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				it->second.Data().clear();
			}
		}

		void UIProfileEditorSlot::DrawProfileEditorMenuBarItems()
		{
			if (ImGui::BeginMenu(UIL::LS(CommonStrings::Actions, "peb_1")))
			{
				EditorDrawMenuBarItems();

				ImGui::EndMenu();
			}
		}

		void UIProfileEditorSlot::DrawMenuBarItemsExtra()
		{
			if (!m_state.selected || !m_cachedItem)
			{
				return;
			}

			if (ImGui::BeginMenu(UIL::LS(UIWidgetCommonStrings::AddSlot, "1")))
			{
				DrawAddSlotMenu();

				ImGui::EndMenu();
			}
		}

		bool UIProfileEditorSlot::ShowConfigClassIndicator() const
		{
			return false;
		}

		bool UIProfileEditorSlot::CreateSlot(Data::ObjectSlot a_slot)
		{
			auto& cachedItem = m_cachedItem;

			if (!cachedItem)
			{
				return false;
			}

			auto& cachedData = cachedItem->data.get(a_slot);

			if (cachedData)
			{
				return false;
			}

			auto& data = GetProfileManager().Data();

			auto it = data.find(cachedItem->name);
			if (it == data.end())
			{
				return false;
			}

			cachedData = CreateDefaultSlotConfig(a_slot, GetConfigClass());

			auto& profileData = it->second.Data().get(a_slot);

			if (profileData)
			{
				*profileData = cachedData->second;
			}
			else
			{
				profileData = std::make_unique<
					Data::configSlotHolder_t::data_type>(
					cachedData->second);
			}

			return true;
		}

		void UIProfileEditorSlot::CreateAllSlots()
		{
			if (const auto& cached = m_cachedItem)
			{
				using enum_type = std::underlying_type_t<Data::ObjectSlot>;

				for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
				{
					const auto slot = static_cast<Data::ObjectSlot>(i);

					if (!cached->data.get(slot))
					{
						CreateSlot(slot);
					}
				}
			}
		}

		void UIProfileEditorSlot::DrawAddSlotMenu()
		{
			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			bool full = true;

			if (const auto& cached = m_cachedItem)
			{
				for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
				{
					const auto slot = static_cast<Data::ObjectSlot>(i);

					if (cached->data.get(slot))
					{
						continue;
					}

					full = false;

					char buf[std::numeric_limits<enum_type>::digits10 + 3];
					stl::snprintf(buf, "%u", i);

					auto& name = StringHolder::GetSingleton().GetSlotName(slot);

					if (ImGui::MenuItem(UIL::LMKID<2>(name.c_str(), buf)))
					{
						CreateSlot(slot);
					}
				}
			}

			if (full)
			{
				ImGui::MenuItem(
					UIL::LS(UIWidgetCommonStrings::NothingToAdd, "ctl_1"),
					nullptr,
					false,
					false);
			}
			else
			{
				ImGui::Separator();
				if (ImGui::MenuItem(UIL::LS(CommonStrings::All, "ctl_1")))
				{
					CreateAllSlots();
				}
			}
		}

		constexpr bool UIProfileEditorSlot::BaseConfigStoreCC() const
		{
			return true;
		}

		WindowLayoutData UIProfileEditorSlot::GetWindowDimensions() const
		{
			return { 150.0f, 600.0f, -1.0f, false };
		}

		constexpr bool UIProfileEditorSlot::IsProfileEditor() const
		{
			return true;
		}

		void UIProfileEditorSlot::DrawItem(SlotProfile& a_profile)
		{
			if (m_cachedItem)
			{
				ImGui::Spacing();
				DrawSlotEditor(0, m_cachedItem->data);
			}
		}

	}
}