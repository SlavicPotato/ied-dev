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
				"IED_pe_slot",
				a_controller),
			UISlotEditorBase<int>(a_controller),
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{}

		void UIProfileEditorSlot::Initialize()
		{
			InitializeProfileBase();

			auto& store = m_controller.GetConfigStore();

			SetSex(store.settings.data.ui.slotProfileEditor.sex, false);
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

		auto UIProfileEditorSlot::GetCurrentData() -> SlotEditorCurrentData
		{
			if (!m_cachedItem)
			{
				return { 0, nullptr };
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

		void UIProfileEditorSlot::OnItemSelected(const stl::fixed_string& a_name)
		{
			//_DMESSAGE("sel: %s", a_name.c_str());

			auto& data = GetProfileManager().Data();

			auto it = data.find(a_name);
			if (it != data.end())
			{
				m_cachedItem.emplace(a_name, it->second.Data());
				ResetFormSelectorWidgets();
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
			SlotProfile& a_profile)
		{
			//_DMESSAGE("save: %s", a_name.c_str());

			if (!m_cachedItem)
			{
				return;
			}

			if (m_cachedItem->name == a_name)
			{
				m_cachedItem = { a_name, a_profile.Data() };
				ResetFormSelectorWidgets();
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
				m_cachedItem = { a_profile.Name(), a_profile.Data() };
				ResetFormSelectorWidgets();
			}
		}

		void UIProfileEditorSlot::OnSexChanged(
			Data::ConfigSex a_newSex)
		{
			auto& store = m_controller.GetConfigStore();

			if (store.settings.data.ui.slotProfileEditor.sex != a_newSex)
			{
				ResetFormSelectorWidgets();
				store.settings.Set(
					store.settings.data.ui.slotProfileEditor.sex,
					a_newSex);
			}
		}

		Data::SettingHolder::EditorPanelCommon& UIProfileEditorSlot::GetEditorPanelSettings()
		{
			return m_controller.GetConfigStore().settings.data.ui.slotProfileEditor;
		}

		void UIProfileEditorSlot::OnEditorPanelSettingsChange()
		{
			auto& store = m_controller.GetConfigStore();
			store.settings.MarkDirty();
		}

		UIData::UICollapsibleStates& UIProfileEditorSlot::GetCollapsibleStatesData()
		{
			auto& config = m_controller.GetConfigStore().settings;

			return config.data.ui.slotProfileEditor.colStates;
		}

		void UIProfileEditorSlot::OnCollapsibleStatesUpdate()
		{
			m_controller.GetConfigStore().settings.MarkDirty();
		}

		void UIProfileEditorSlot::OnBaseConfigChange(
			int,
			const void* a_params,
			PostChangeAction a_action)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				auto i = stl::underlying(params->slot);

				auto sync = GetEditorPanelSettings().sexSync;
				auto sex = GetSex();

				auto& src = params->entry.data;
				auto& dst = it->second.Data().get(params->slot);

				if (sync)
				{
					src->get(Data::GetOppositeSex(sex)) = src->get(sex);
				}

				if (!dst)
				{
					dst = std::make_unique<Data::configSlotHolder_t::data_type>(*src);
				}
				else
				{
					if (sync)
					{
						*dst = *src;
					}
					else
					{
						dst->get(sex) = src->get(sex);
					}
				}
			}
		}

		void UIProfileEditorSlot::OnFullConfigChange(
			int,
			const SlotConfigUpdateParams& a_params)
		{
			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				Data::configSlotHolder_t tmp;

				using enum_type = std::underlying_type_t<Data::ObjectSlot>;

				for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
				{
					auto& src = a_params.data.entries[i].data;

					if (src)
					{
						tmp.get(static_cast<Data::ObjectSlot>(i)) =
							std::make_unique<Data::configSlotHolder_t::data_type>(*src);
					}
				}

				it->second.Data() = std::move(tmp);
			}

			ResetFormSelectorWidgets();
		}

		void UIProfileEditorSlot::OnSingleSlotClear(int, const void* a_params)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it != data.end())
			{
				it->second.Data().get(params->slot).reset();
			}
		}

		void UIProfileEditorSlot::OnFullConfigClear(int a_handle)
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
			if (ImGui::BeginMenu(LS(CommonStrings::Slot, "1")))
			{
				DrawMenuBarItems();

				ImGui::EndMenu();
			}
		}

		void UIProfileEditorSlot::DrawMenuBarItemsExtra()
		{
			if (!m_cachedItem)
			{
				return;
			}

			if (ImGui::BeginMenu(LS(UIWidgetCommonStrings::AddSlot, "1")))
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
			if (!m_cachedItem)
			{
				return false;
			}

			auto& cachedData = m_cachedItem->data.get(a_slot);

			if (cachedData.data)
			{
				return false;
			}

			auto& data = GetProfileManager().Data();

			auto it = data.find(m_cachedItem->name);
			if (it == data.end())
			{
				return false;
			}

			cachedData.data = CreateDefaultSlotConfig(a_slot);

			auto& profileData = it->second.Data().get(a_slot);

			if (profileData)
			{
				*profileData = *cachedData.data;
			}
			else
			{
				profileData = std::make_unique<Data::configSlotHolder_t::data_type>(
					*cachedData.data);
			}

			return true;
		}

		void UIProfileEditorSlot::CreateAllSlots()
		{
			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
			{
				auto& d = m_cachedItem->data.entries[i];

				if (!d.data)
				{
					CreateSlot(static_cast<Data::ObjectSlot>(i));
				}
			}
		}

		void UIProfileEditorSlot::DrawAddSlotMenu()
		{
			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			bool full = true;

			for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
			{
				auto& d = m_cachedItem->data.entries[i];

				if (d.data)
				{
					continue;
				}

				full = false;

				auto slot = static_cast<Data::ObjectSlot>(i);

				auto& name = StringHolder::GetSingleton().GetSlotName(slot);

				char buf[std::numeric_limits<enum_type>::digits10 + 3];
				stl::snprintf(buf, "%u", i);

				if (ImGui::MenuItem(LMKID<2>(name.c_str(), buf)))
				{
					CreateSlot(slot);
				}
			}

			if (full)
			{
				ImGui::MenuItem(
					LS(UIWidgetCommonStrings::NothingToAdd, "ctl_1"),
					nullptr,
					false,
					false);
			}
			else
			{
				ImGui::Separator();
				if (ImGui::MenuItem(LS(CommonStrings::All, "ctl_1")))
				{
					CreateAllSlots();
				}
			}
		}

		constexpr bool UIProfileEditorSlot::BaseConfigStoreCC() const
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

	}  // namespace UI
}  // namespace IED