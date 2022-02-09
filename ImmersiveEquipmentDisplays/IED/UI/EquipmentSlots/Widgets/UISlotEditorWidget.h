#pragma once

#include "IED/UI/PopupQueue/UIPopupQueue.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIEditorInterface.h"
#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/UISettingsInterface.h"
#include "IED/UI/Widgets/Filters/UIGenericFilter.h"
#include "IED/UI/Widgets/Form/UIFormFilterWidget.h"
#include "IED/UI/Widgets/Form/UIFormPickerWidget.h"
#include "IED/UI/Widgets/UIBaseConfigWidget.h"
#include "IED/UI/Widgets/UICurrentData.h"
#include "IED/UI/Widgets/UIEditorPanelSettingsGear.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UIProfileSelectorWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/ConfigOverride.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "UISlotEditorWidgetStrings.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		using entrySlotData_t = Data::configSlotHolderCopy_t;

		template <class T>
		struct profileSelectorParamsSlot_t
		{
			T handle;
			entrySlotData_t& data;
		};

		struct SingleSlotConfigUpdateParams
		{
			Data::ObjectSlot slot;
			Data::ConfigSex sex;
			entrySlotData_t::data_type& entry;
		};

		struct SingleSlotConfigClearParams
		{
			Data::ObjectSlot slot;
			Data::ConfigSex sex;
			entrySlotData_t& data;
		};

		struct FullSlotConfigClearParams
		{
			entrySlotData_t& data;
		};

		struct SlotConfigUpdateParams
		{
			entrySlotData_t& data;
		};

		enum class FormEntryAction : std::uint8_t
		{
			None,
			Add,
			Swap,
			Delete,
			Copy,
			Paste
		};

		struct FormEntryActionResult
		{
			FormEntryAction action{ FormEntryAction::None };
			Game::FormID form;
			SwapDirection dir;
		};

		template <class T>
		struct slotFormFilterParams_t
		{
			T handle;
			SingleSlotConfigUpdateParams& params;
		};

		template <class T>
		class UISlotEditorWidget :
			public UIEditorInterface,
			public UIEditorPanelSettingsGear,
			public UIBaseConfigWidget<T>,
			public virtual UISettingsInterface,
			public virtual UIPopupToggleButtonWidget
		{
		public:
			UISlotEditorWidget(
				Controller& a_controller);

			void DrawSlotEditor(
				T a_handle,
				entrySlotData_t& a_data);

			virtual void DrawMenuBarItems() override;

			virtual const char* GetDisplayName() const;

		protected:
			using SlotEditorCurrentData = UICurrentData<T, entrySlotData_t>;

			virtual constexpr Data::ConfigClass GetConfigClass() const = 0;

			void DrawSlotEditorNPCWarningHeader();

		private:
			virtual bool ShowConfigClassIndicator() const;
			virtual bool PermitDeletion() const;

			virtual bool DrawExtraSlotInfo(
				T a_handle,
				Data::ObjectSlot a_slot,
				const entrySlotData_t::data_type& a_entry,
				bool a_infoDrawn);

			virtual void DrawExtraFlags(
				T a_handle,
				Data::configBaseValues_t& a_data,
				Data::configBase_t* a_baseConfig,
				const void* a_params) override;

			virtual UIPopupQueue& GetPopupQueue() = 0;
			virtual SlotEditorCurrentData GetCurrentData() = 0;

			virtual void
				OnFullConfigChange(
					T a_handle,
					const SlotConfigUpdateParams& a_params) = 0;

			virtual void OnSingleSlotClear(
				T a_handle,
				const SingleSlotConfigClearParams& a_params) = 0;

			virtual void OnFullConfigClear(
				T a_handle,
				const FullSlotConfigClearParams& a_params) = 0;

			virtual void DrawMainHeaderControlsExtra(
				T a_handle,
				entrySlotData_t& a_data);

			virtual void DrawMenuBarItemsExtra();

			virtual bool GetEnableEquipmentOverridePropagation() override;

			void DrawSlotConfig(
				T a_handle,
				SingleSlotConfigUpdateParams& a_params);

			void DrawPreferredItemsTree(
				T a_handle,
				SingleSlotConfigUpdateParams& a_params,
				Data::configSlot_t& a_data);

			void DrawPreferredItemsTable(
				T a_handle,
				SingleSlotConfigUpdateParams& a_params,
				Data::configFormList_t& a_data);

			void DrawEditPreferredItemEntryFormPopup(
				T a_handle,
				SingleSlotConfigUpdateParams& a_params,
				Game::FormID& a_out);

			void DrawFormInfoText(Game::FormID a_form);

			FormEntryActionResult DrawPreferredItemEntryContextMenu();

			FormEntryActionResult DrawPreferredItemsHeaderContextMenu(
				T a_handle,
				SingleSlotConfigUpdateParams& a_params,
				Data::configFormList_t& a_data);

			void QueueCopySlotSexPopup(
				T a_handle,
				Data::ConfigSex a_ssex,
				Data::ObjectSlot a_slot);

			void QueueClearSlot(
				T a_handle,
				Data::ConfigSex a_sex,
				Data::ObjectSlot a_slot);

			void QueueSetAllSlotsEnabled(
				T a_handle,
				Data::ConfigSex a_sex,
				bool a_switch);

			void DrawSlotContextMenu(
				T a_handle,
				const SingleSlotConfigUpdateParams& a_params);

			void DrawSlotHeaderControls(
				T a_handle,
				const SingleSlotConfigUpdateParams& a_params);

			void DrawSlotEntry(
				T a_handle,
				Data::ObjectSlot a_slot,
				entrySlotData_t::data_type& a_entry);

			void DrawSlotFilter();

			[[nodiscard]] inline static constexpr bool Is2HLeftSlot(
				Data::ObjectSlot a_slot) noexcept
			{
				return a_slot == Data::ObjectSlot::k2HSwordLeft ||
				       a_slot == Data::ObjectSlot::k2HAxeLeft;
			}

		private:
			UIGenericFilter m_slotFilter;
			UIFormSelectorWidget m_formSelector;
			UIFormFilterWidget<slotFormFilterParams_t<T>> m_formFilter;

			Game::FormID m_ffNewEntryID;
			Game::FormID m_piNewEntryID;
			Game::FormID m_piEditEntryID;
		};

		template <class T>
		UISlotEditorWidget<T>::UISlotEditorWidget(
			Controller& a_controller) :
			UIBaseConfigWidget<T>(a_controller),
			UISettingsInterface(a_controller),
			UIEditorPanelSettingsGear(a_controller),
			m_formSelector(a_controller, FormInfoFlags::kValidSlot, true, true, false),
			m_formFilter(a_controller, m_formSelector),
			m_slotFilter(true)
		{
			m_formSelector.SetAllowedTypes(
				{ TESObjectWEAP::kTypeID,
			      TESObjectARMO::kTypeID,
			      TESObjectLIGH::kTypeID });

			m_formFilter.SetOnChangeFunc([this](slotFormFilterParams_t<T>& a_params) {
				OnBaseConfigChange(
					a_params.handle,
					std::addressof(a_params.params),
					PostChangeAction::Evaluate);
			});
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotEditorNPCWarningHeader()
		{
			ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
			ImGui::TextWrapped("%s", LS(UISlotEditorWidgetStrings::NPCDisabledWarning));
			ImGui::PopStyleColor();
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
		}

		template <class T>
		bool UISlotEditorWidget<T>::ShowConfigClassIndicator() const
		{
			return true;
		}

		template <class T>
		bool UISlotEditorWidget<T>::PermitDeletion() const
		{
			return true;
		}

		template <class T>
		bool UISlotEditorWidget<T>::DrawExtraSlotInfo(
			T a_handle,
			Data::ObjectSlot a_slot,
			const entrySlotData_t::data_type& a_entry,
			bool a_infoDrawn)
		{
			return false;
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawExtraFlags(
			T a_handle,
			Data::configBaseValues_t& a_data,
			Data::configBase_t* a_baseConfig,
			const void* a_params)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			ImGui::Separator();

			auto& data = params->entry.second.get(params->sex);

			const bool disabled = data.flags.test(Data::FlagsBase::kDisabled) &&
			                      data.equipmentOverrides.empty();

			UICommon::PushDisabled(disabled);

			ImGui::Columns(2, nullptr, false);

			if (ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::AlwaysUnload, "1"),
					stl::underlying(std::addressof(data.slotFlags.value)),
					stl::underlying(Data::SlotFlags::kAlwaysUnload)))
			{
				OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
			}

			DrawTip(UITip::AlwaysUnloadSlot);

			ImGui::NextColumn();

			if (ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::CheckCannotWear, "2"),
					stl::underlying(std::addressof(data.slotFlags.value)),
					stl::underlying(Data::SlotFlags::kCheckCannotWear)))
			{
				OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
			}

			DrawTip(UITip::CheckCannotWear);

			ImGui::Columns();

			UICommon::PopDisabled(disabled);
		}

		template <class T>
		const char* UISlotEditorWidget<T>::GetDisplayName() const
		{
			return GetConfigClassName(GetConfigClass());
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotEditor(
			T a_handle,
			entrySlotData_t& a_data)
		{
			ImGui::PushID("slot_editor_widget");

			DrawEditorPanelSettings();

			ImGui::Spacing();

			ImGui::PushID("extra_header");

			DrawMainHeaderControlsExtra(a_handle, a_data);

			ImGui::PopID();

			ImGui::Separator();

			DrawSlotFilter();

			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::BeginChild("slot_editor_panel", { -1.0f, 0.0f }))
			{
				ImGui::PushItemWidth(ImGui::GetFontSize() * -10.5f);

				using enum_type = std::underlying_type_t<Data::ObjectSlot>;

				for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
				{
					auto slot = static_cast<Data::ObjectSlot>(i);

					auto& slotName = StringHolder::GetSingleton().GetSlotName(slot);

					if (!m_slotFilter.Test(slotName))
					{
						continue;
					}

					auto& entry = a_data.data[i];

					if (entry)
					{
						ImGui::PushID(i);

						DrawSlotEntry(a_handle, slot, *entry);

						ImGui::PopID();

						ImGui::Spacing();
					}
				}

				ImGui::PopItemWidth();
			}

			ImGui::EndChild();

			ImGui::PopID();
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawMenuBarItems()
		{
			auto sex = GetSex();

			auto current = GetCurrentData();

			bool disabled = !current;

			UICommon::PushDisabled(disabled);

			if (LCG_MI(UIWidgetCommonStrings::CopyAllFromOppositeSex, "1"))
			{
				auto& queue = GetPopupQueue();

				queue.push(
						 UIPopupType::Confirm,
						 LS(CommonStrings::Confirm),
						 "%s",
						 LS(UIWidgetCommonStrings::CopyAllFromOppositeSexPrompt))
					.call([this,
				           ssex = GetOppositeSex(sex),
				           tsex = sex](
							  const auto&) {
						auto current = GetCurrentData();
						if (!current)
						{
							return;
						}

						auto configClass = GetConfigClass();

						for (auto& e : current.data->data)
						{
							if (!e)
							{
								continue;
							}

							if (e->first != configClass)
							{
								continue;
							}

							e->second.get(tsex) = e->second.get(ssex);
						}

						SlotConfigUpdateParams params{ *current.data };

						OnFullConfigChange(current.handle, params);
					});
			}

			if (PermitDeletion())
			{
				if (LCG_MI(UISlotEditorWidgetStrings::ClearAllSlots, "2"))
				{
					auto& queue = GetPopupQueue();

					queue.push(
							 UIPopupType::Confirm,
							 LS(CommonStrings::Confirm),
							 "%s",
							 LS(UISlotEditorWidgetStrings::ClearAllSlotsPrompt))
						.call([this](const auto&) {
							if (auto current = GetCurrentData())
							{
								for (auto& e : current.data->data)
								{
									e.reset();
								}

								OnFullConfigClear(current.handle, { *current.data });
							}
						});
				}
			}

			ImGui::Separator();

			if (LCG_BM(UISlotEditorWidgetStrings::AllSlots, "3"))
			{
				if (LCG_MI(CommonStrings::Enable, "1"))
				{
					QueueSetAllSlotsEnabled(current.handle, sex, true);
				}

				if (LCG_MI(CommonStrings::Disable, "2"))
				{
					QueueSetAllSlotsEnabled(current.handle, sex, false);
				}

				ImGui::EndMenu();
			}

			UICommon::PopDisabled(disabled);

			DrawMenuBarItemsExtra();
		}

		template <class T>
		void UISlotEditorWidget<T>::QueueCopySlotSexPopup(
			T a_handle,
			Data::ConfigSex a_tsex,
			Data::ObjectSlot a_slot)
		{
			auto& queue = GetPopupQueue();
			auto& slotName = StringHolder::GetSingleton().GetSlotName(a_slot);

			queue.push(
					 UIPopupType::Confirm,
					 LS(CommonStrings::Confirm),
					 "%s",
					 LS(UISlotEditorWidgetStrings::CopyOppositeSexSlotPrompt))
				.call([this,
			           handle = a_handle,
			           slot = a_slot,
			           tsex = a_tsex,
			           ssex = GetOppositeSex(a_tsex)](
						  const auto&) {
					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					if (handle != current.handle)
					{
						return;
					}

					auto& slotData = current.data->get(slot);
					if (!slotData)
					{
						return;
					}

					slotData->second.get(tsex) = slotData->second.get(ssex);

					SingleSlotConfigUpdateParams params{ slot, tsex, *slotData };

					OnBaseConfigChange(
						handle,
						std::addressof(params),
						PostChangeAction::Reset);
				});
		}

		template <class T>
		void UISlotEditorWidget<T>::QueueClearSlot(
			T a_handle,
			Data::ConfigSex a_sex,
			Data::ObjectSlot a_slot)
		{
			auto& queue = GetPopupQueue();
			auto& slotName = StringHolder::GetSingleton().GetSlotName(a_slot);

			queue.push(
					 UIPopupType::Confirm,
					 LS(CommonStrings::Confirm),
					 "%s",
					 LS(UISlotEditorWidgetStrings::ClearSlotPrompt))
				.call([this, a_handle, a_sex, a_slot](
						  const auto&) {
					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					if (a_handle != current.handle)
					{
						return;
					}

					auto& slotData = current.data->get(a_slot);
					if (!slotData)
					{
						return;
					}

					if (slotData->first != GetConfigClass())
					{
						return;
					}

					slotData.reset();

					OnSingleSlotClear(a_handle, { a_slot, a_sex, *current.data });
				});
		}

		template <class T>
		void UISlotEditorWidget<T>::QueueSetAllSlotsEnabled(
			T a_handle,
			Data::ConfigSex a_sex,
			bool a_switch)
		{
			auto& queue = GetPopupQueue();

			queue.push(
					 UIPopupType::Confirm,
					 LS(CommonStrings::Confirm),
					 "%s",
					 a_switch ?
                         LS(UISlotEditorWidgetStrings::EnableAllSlotsPrompt) :
                         LS(UISlotEditorWidgetStrings::DisableAllSlotsPrompt))
				.call([this, a_handle, a_sex, a_switch](
						  const auto&) {
					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					if (a_handle != current.handle)
					{
						return;
					}

					using enum_type = std::underlying_type_t<Data::ObjectSlot>;

					for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
					{
						auto slot = static_cast<Data::ObjectSlot>(i);

						if (Is2HLeftSlot(slot))
						{
							continue;
						}

						auto& slotData = current.data->get(slot);
						if (!slotData)
						{
							continue;
						}

						slotData->first = GetConfigClass();

						auto& data = slotData->second.get(a_sex);

						if (a_switch)
						{
							data.flags.clear(Data::FlagsBase::kDisabled);
						}
						else
						{
							data.flags.set(Data::FlagsBase::kDisabled);
						}

						SingleSlotConfigUpdateParams params{ slot, a_sex, *slotData };

						OnBaseConfigChange(
							a_handle,
							std::addressof(params),
							PostChangeAction::Evaluate);
					}
				});
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotContextMenu(
			T a_handle,
			const SingleSlotConfigUpdateParams& a_params)
		{
			if (ImGui::MenuItem(LS(UIWidgetCommonStrings::CopyFromOppositeSex, "1")))
			{
				QueueCopySlotSexPopup(
					a_handle,
					a_params.sex,
					a_params.slot);
			}

			if (a_params.entry.first == GetConfigClass())
			{
				if (PermitDeletion())
				{
					if (ImGui::MenuItem(LS(UISlotEditorWidgetStrings::ClearSlot, "2")))
					{
						QueueClearSlot(a_handle, a_params.sex, a_params.slot);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem(LS(CommonStrings::Copy, "3")))
			{
				UIClipboard::Set(a_params.entry.second.get(a_params.sex));
			}

			auto clipData = UIClipboard::Get<Data::configSlot_t>();

			if (ImGui::MenuItem(
					LS(CommonStrings::Paste, "4"),
					nullptr,
					false,
					clipData != nullptr))
			{
				if (clipData)
				{
					a_params.entry.second.get(a_params.sex) = *clipData;

					OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Reset);
				}
			}
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotHeaderControls(
			T a_handle,
			const SingleSlotConfigUpdateParams& a_params)
		{
			ImGui::PushID("header_controls");

			DrawPopupToggleButton("open", "context_menu");

			if (ImGui::BeginPopup("context_menu"))
			{
				DrawSlotContextMenu(a_handle, a_params);
				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotEntry(
			T a_handle,
			Data::ObjectSlot a_slot,
			entrySlotData_t::data_type& a_entry)
		{
			SingleSlotConfigUpdateParams params{ a_slot, GetSex(), a_entry };

			DrawSlotHeaderControls(a_handle, params);

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			auto& slotName = StringHolder::GetSingleton().GetSlotName(a_slot);

			if (CollapsingHeader(
					"tree_slot",
					stl::underlying(a_slot) == 0,
					"%s",
					slotName.c_str()))
			{
				ImGui::Spacing();

				ImGui::Indent();

				bool infoDrawn;

				if (infoDrawn = ShowConfigClassIndicator())
				{
					ImGui::Text("%s:", LS(UIWidgetCommonStrings::ConfigInUse));
					ImGui::SameLine();
					DrawConfigClassInUse(a_entry.first);
				}

				ImGui::PushID("extra_slot_info");

				infoDrawn |= DrawExtraSlotInfo(a_handle, a_slot, a_entry, infoDrawn);

				ImGui::PopID();

				if (infoDrawn)
				{
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
				}

				if (Is2HLeftSlot(a_slot))
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
					ImGui::TextWrapped(
						"%s",
						LS(UISlotEditorWidgetStrings::Left2HSlotWarning));
					ImGui::PopStyleColor();

					ImGui::Separator();
					ImGui::Spacing();
				}

				auto& data = params.entry.second.get(params.sex);
				auto& sh = StringHolder::GetSingleton();

				ImGui::PushID("base_config");

				DrawBaseConfig(
					a_handle,
					data,
					std::addressof(params),
					sh.GetSlotName(params.slot));

				ImGui::PopID();

				ImGui::PushID("slot_config");

				DrawSlotConfig(a_handle, params);

				ImGui::PopID();

				ImGui::Unindent();

				ImGui::Spacing();
			}
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawMainHeaderControlsExtra(
			T a_handle,
			entrySlotData_t& a_data)
		{}

		template <class T>
		void UISlotEditorWidget<T>::DrawMenuBarItemsExtra()
		{}

		template <class T>
		inline bool UISlotEditorWidget<T>::GetEnableEquipmentOverridePropagation()
		{
			return GetEditorPanelSettings().eoPropagation;
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotConfig(
			T a_handle,
			SingleSlotConfigUpdateParams& a_params)
		{
			auto& data = a_params.entry.second.get(a_params.sex);

			const bool disabled = data.flags.test(Data::FlagsBase::kDisabled) &&
			                      data.equipmentOverrides.empty();

			UICommon::PushDisabled(disabled);

			ImGui::PushID("pref_items_tree");
			DrawPreferredItemsTree(a_handle, a_params, data);
			ImGui::PopID();

			ImGui::PushID("form_filters_tree");

			slotFormFilterParams_t<T> ffparams{
				a_handle,
				a_params
			};

			m_formFilter.DrawFormFiltersTree(
				LS(UIWidgetCommonStrings::ItemFilters),
				ffparams,
				data.itemFilter);

			ImGui::PopID();

			UICommon::PopDisabled(disabled);
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawPreferredItemsTree(
			T a_handle,
			SingleSlotConfigUpdateParams& a_params,
			Data::configSlot_t& a_data)
		{
			const auto result = DrawPreferredItemsHeaderContextMenu(
				a_handle,
				a_params,
				a_data.preferredItems);

			bool empty = a_data.preferredItems.empty();

			UICommon::PushDisabled(empty);

			if (!empty)
			{
				if (result.action == FormEntryAction::Add ||
				    result.action == FormEntryAction::Paste)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			if (TreeEx(
					"pi_tree",
					false,
					"%s",
					LS(UIWidgetCommonStrings::PreferredItems)))
			{
				if (!empty)
				{
					ImGui::Spacing();

					DrawPreferredItemsTable(a_handle, a_params, a_data.preferredItems);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(empty);
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawPreferredItemsTable(
			T a_handle,
			SingleSlotConfigUpdateParams& a_params,
			Data::configFormList_t& a_data)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 5.f, 5.f });

			constexpr int NUM_COLUMNS = 3;

			if (ImGui::BeginTable(
					"form_list_table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.f }))
			{
				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, ImGui::GetFontSize() * 4.0f);
				ImGui::TableSetupColumn(LS(CommonStrings::FormID), ImGuiTableColumnFlags_None, 75.0f);
				ImGui::TableSetupColumn(LS(CommonStrings::Info), ImGuiTableColumnFlags_None, 250.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				int i = 0;

				auto it = a_data.begin();
				while (it != a_data.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					auto result = DrawPreferredItemEntryContextMenu();
					switch (result.action)
					{
					case FormEntryAction::Add:
						it = a_data.emplace(it, result.form);

						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);

						ImGui::SetNextItemOpen(true);

						break;
					case FormEntryAction::Delete:
						it = a_data.erase(it);

						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);

						break;
					case FormEntryAction::Swap:
						if (IterSwap(a_data, it, result.dir))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
						break;
					}

					if (it != a_data.end())
					{
						ImGui::TableSetColumnIndex(1);

						char buf[24];

						stl::snprintf(buf, "%.8X##form", it->get());

						if (ImGui::Selectable(
								buf,
								false,
								ImGuiSelectableFlags_DontClosePopups))
						{
							ImGui::OpenPopup("form_edit_context_menu");
							m_piEditEntryID = *it;
						}

						DrawEditPreferredItemEntryFormPopup(a_handle, a_params, *it);

						ImGui::TableSetColumnIndex(2);
						DrawFormInfoText(*it);

						++it;
						i++;
					}

					ImGui::PopID();
				}

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawEditPreferredItemEntryFormPopup(
			T a_handle,
			SingleSlotConfigUpdateParams& a_params,
			Game::FormID& a_out)
		{
			if (ImGui::BeginPopup("form_edit_context_menu"))
			{
				if (m_formSelector.DrawFormSelector(
						LS(CommonStrings::Form, "fs"),
						m_piEditEntryID))
				{
					if (m_piEditEntryID)
					{
						a_out = m_piEditEntryID;
						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);
					}

					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawFormInfoText(Game::FormID a_form)
		{
			if (auto formInfo = LookupForm(a_form))
			{
				if (auto typeDesc = IFormCommon::GetFormTypeDesc(formInfo->form.type))
				{
					ImGui::Text(
						"[%s] %s",
						typeDesc,
						formInfo->form.name.c_str());
				}
				else
				{
					ImGui::Text(
						"[%hhu] %s",
						formInfo->form.type,
						formInfo->form.name.c_str());
				}
			}
		}

		template <class T>
		FormEntryActionResult UISlotEditorWidget<T>::DrawPreferredItemEntryContextMenu()
		{
			FormEntryActionResult result;

			ImGui::PushID("pi_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
			{
				m_piNewEntryID = {};
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = FormEntryAction::Swap;
				result.dir = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = FormEntryAction::Swap;
				result.dir = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Insert, "1"))
				{
					if (m_formSelector.DrawFormSelector(
							LS(CommonStrings::Form, "fs"),
							m_piNewEntryID))
					{
						if (m_piNewEntryID)
						{
							result.action = FormEntryAction::Add;
							result.form = m_piNewEntryID;
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (LCG_MI(CommonStrings::Delete, "2"))
				{
					result.action = FormEntryAction::Delete;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		FormEntryActionResult UISlotEditorWidget<T>::DrawPreferredItemsHeaderContextMenu(
			T a_handle,
			SingleSlotConfigUpdateParams& a_params,
			Data::configFormList_t& a_data)
		{
			FormEntryActionResult result;

			ImGui::PushID("pi_header_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
			{
				m_piNewEntryID = 0;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::Add, "1"))
				{
					if (m_formSelector.DrawFormSelector(
							LS(CommonStrings::Form, "fs"),
							m_piNewEntryID))
					{
						a_data.emplace_back(m_piNewEntryID);

						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);

						ImGui::CloseCurrentPopup();

						result.action = FormEntryAction::Add;
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configFormList_t>();

				if (ImGui::MenuItem(
						LS(CommonStrings::Paste, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);

						result.action = FormEntryAction::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotFilter()
		{
			if (TreeEx(
					"slot_filter",
					false,
					"%s",
					LS(CommonStrings::Filter)))
			{
				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				ImGui::Spacing();

				m_slotFilter.Draw();

				ImGui::Spacing();

				ImGui::PopItemWidth();

				ImGui::TreePop();
			}
		}

	}
}