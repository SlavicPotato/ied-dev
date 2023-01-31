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
#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UIProfileSelectorWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/ConfigStore.h"
#include "IED/Data.h"
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
			T                handle;
			entrySlotData_t& data;
		};

		struct SingleSlotConfigUpdateParams
		{
			Data::ObjectSlot            slot;
			Data::ConfigSex             sex;
			entrySlotData_t::data_type& entry;
		};

		struct SingleSlotConfigClearParams
		{
			Data::ObjectSlot slot;
			Data::ConfigSex  sex;
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

		struct SlotPriorityConfigUpdateParams
		{
			Data::ConfigSex                  sex;
			entrySlotData_t::prio_data_type& entry;
		};

		enum class SlotContextAction : std::uint8_t
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
			SlotContextAction action{ SlotContextAction::None };
			Game::FormID      form;
			SwapDirection     dir;
		};

		template <class T>
		struct slotFormFilterParams_t
		{
			T                             handle;
			SingleSlotConfigUpdateParams& params;
		};

		template <class T>
		class UISlotEditorWidget :
			public UIBaseConfigWidget<T>,
			public UIEditorPanelSettingsGear,
			public virtual UISettingsInterface,
			public virtual UIFormTypeSelectorWidget,
			public UIEditorInterface
		{
		public:
			UISlotEditorWidget(
				Controller& a_controller);

			void DrawSlotEditor(
				T                a_handle,
				entrySlotData_t& a_data);

			virtual void EditorDrawMenuBarItems() override;

			virtual const char* GetDisplayName() const;

		protected:
			using SlotEditorCurrentData = UICurrentData<T, entrySlotData_t>;

			virtual constexpr Data::ConfigClass GetConfigClass() const = 0;

			void DrawSlotEditorNPCWarningHeader();

		private:
			void DrawSlotEntries(
				T                                 a_handle,
				entrySlotData_t::array_data_type& a_data);

			SlotContextAction DrawPriorityContextMenu(
				T                a_handle,
				entrySlotData_t& a_data);

			void DrawPriority(
				T                a_handle,
				entrySlotData_t& a_data);

			void DrawPriorityEntry(
				T                                a_handle,
				entrySlotData_t::prio_data_type& a_data);

			virtual bool ShowConfigClassIndicator() const;
			virtual bool PermitDeletion() const;

			virtual bool DrawExtraSlotInfo(
				T                                 a_handle,
				Data::ObjectSlot                  a_slot,
				const entrySlotData_t::data_type& a_entry,
				bool                              a_infoDrawn);

			virtual void DrawExtraFlags(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				Data::configBase_t*       a_baseConfig,
				const void*               a_params) override;

			virtual UIPopupQueue&         GetPopupQueue()  = 0;
			virtual SlotEditorCurrentData GetCurrentData() = 0;

			virtual void OnFullConfigChange(
				T                             a_handle,
				const SlotConfigUpdateParams& a_params) = 0;

			virtual void OnPriorityConfigChange(
				T                                     a_handle,
				const SlotPriorityConfigUpdateParams& a_params) = 0;

			virtual void
				OnPriorityConfigClear(
					T                             a_handle,
					const SlotConfigUpdateParams& a_params) = 0;

			virtual void OnSingleSlotClear(
				T                                  a_handle,
				const SingleSlotConfigClearParams& a_params) = 0;

			virtual void OnFullConfigClear(
				T                                a_handle,
				const FullSlotConfigClearParams& a_params) = 0;

			virtual void DrawMainHeaderControlsExtra(
				T                a_handle,
				entrySlotData_t& a_data);

			virtual void DrawMenuBarItemsExtra();

			virtual bool GetEnableEquipmentOverridePropagation() override;

			void DrawSlotConfig(
				T                             a_handle,
				SingleSlotConfigUpdateParams& a_params);

			void DrawPreferredItemsTree(
				T                             a_handle,
				SingleSlotConfigUpdateParams& a_params,
				Data::configSlot_t&           a_data);

			void DrawPreferredItemsTable(
				T                             a_handle,
				SingleSlotConfigUpdateParams& a_params,
				Data::configFormList_t&       a_data);

			void DrawEditPreferredItemEntryFormPopup(
				T                             a_handle,
				SingleSlotConfigUpdateParams& a_params,
				Game::FormID&                 a_out);

			void DrawFormInfoText(Game::FormID a_form);

			FormEntryActionResult DrawPreferredItemEntryContextMenu();

			FormEntryActionResult DrawPreferredItemsHeaderContextMenu(
				T                             a_handle,
				SingleSlotConfigUpdateParams& a_params,
				Data::configFormList_t&       a_data);

			void QueueCopySlotSexPopup(
				T                a_handle,
				Data::ConfigSex  a_ssex,
				Data::ObjectSlot a_slot);

			void QueueClearSlot(
				T                a_handle,
				Data::ConfigSex  a_sex,
				Data::ObjectSlot a_slot);

			void QueueSetAllSlotsEnabled(
				T               a_handle,
				Data::ConfigSex a_sex,
				bool            a_switch);

			void DrawSlotContextMenu(
				T                                   a_handle,
				const SingleSlotConfigUpdateParams& a_params);

			void DrawSlotHeaderControls(
				T                                   a_handle,
				const SingleSlotConfigUpdateParams& a_params);

			void DrawSlotEntry(
				T                           a_handle,
				Data::ObjectSlot            a_slot,
				entrySlotData_t::data_type& a_entry);

			void DrawSlotFilter();

			[[nodiscard]] static constexpr bool Is2HLeftSlot(
				Data::ObjectSlot a_slot) noexcept
			{
				return a_slot == Data::ObjectSlot::k2HSwordLeft ||
				       a_slot == Data::ObjectSlot::k2HAxeLeft;
			}

		private:
			UIGenericFilter                               m_slotFilter;
			UIFormSelectorWidget                          m_formSelector;
			UIFormFilterWidget<slotFormFilterParams_t<T>> m_formFilter;

			Game::FormID m_ffNewEntryID;
			Game::FormID m_piNewEntryID;
			Game::FormID m_piEditEntryID;
		};

		template <class T>
		UISlotEditorWidget<T>::UISlotEditorWidget(
			Controller& a_controller) :
			UIBaseConfigWidget<T>(a_controller),
			m_formSelector(
				a_controller,
				FormInfoFlags::kValidSlot,
				true,
				true,
				false),
			m_formFilter(a_controller, m_formSelector),
			m_slotFilter(true)
		{
			m_formSelector.SetAllowedTypes(
				{ TESObjectWEAP::kTypeID,
			      TESObjectARMO::kTypeID,
			      TESObjectLIGH::kTypeID,
			      TESAmmo::kTypeID });

			m_formFilter.SetOnChangeFunc([this](slotFormFilterParams_t<T>& a_params) {
				this->OnBaseConfigChange(
					a_params.handle,
					std::addressof(a_params.params),
					PostChangeAction::Evaluate);
			});
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotEditorNPCWarningHeader()
		{
			ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorWarning);
			ImGui::TextWrapped("%s", UIL::LS(UISlotEditorWidgetStrings::NPCDisabledWarning));
			ImGui::PopStyleColor();
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
			T                                 a_handle,
			Data::ObjectSlot                  a_slot,
			const entrySlotData_t::data_type& a_entry,
			bool                              a_infoDrawn)
		{
			return false;
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawExtraFlags(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			Data::configBase_t*       a_baseConfig,
			const void*               a_params)
		{
			auto params = static_cast<const SingleSlotConfigUpdateParams*>(a_params);

			auto& data = params->entry.second.get(params->sex);

			ImGui::Columns(2, nullptr, false);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::AlwaysUnload, "1"),
					stl::underlying(std::addressof(data.slotFlags.value)),
					stl::underlying(Data::SlotFlags::kAlwaysUnload)))
			{
				this->OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
			}
			UITipsInterface::DrawTip(UITip::AlwaysUnloadSlot);

			ImGui::NextColumn();

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::CheckCannotWear, "2"),
					stl::underlying(std::addressof(data.slotFlags.value)),
					stl::underlying(Data::SlotFlags::kCheckCannotWear)))
			{
				this->OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
			}
			UITipsInterface::DrawTip(UITip::CheckCannotWear);

			ImGui::Columns();
		}

		template <class T>
		const char* UISlotEditorWidget<T>::GetDisplayName() const
		{
			return Data::GetConfigClassName(GetConfigClass());
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotEditor(
			T                a_handle,
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

				DrawPriority(a_handle, a_data);

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				DrawSlotEntries(a_handle, a_data.data);

				ImGui::PopItemWidth();
			}

			ImGui::EndChild();

			ImGui::PopID();
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotEntries(
			T                                 a_handle,
			entrySlotData_t::array_data_type& a_data)
		{
			ImGui::PushID("slots");

			using enum_type = std::underlying_type_t<Data::ObjectSlot>;

			for (enum_type i = 0; i < stl::underlying(Data::ObjectSlot::kMax); i++)
			{
				auto slot = static_cast<Data::ObjectSlot>(i);

				auto& slotName = StringHolder::GetSingleton().GetSlotName(slot);

				if (!m_slotFilter.Test(*slotName))
				{
					continue;
				}

				auto& entry = a_data[i];

				if (entry)
				{
					ImGui::PushID(i);

					DrawSlotEntry(a_handle, slot, *entry);

					ImGui::PopID();

					ImGui::Spacing();
				}
			}

			ImGui::PopID();
		}

		template <class T>
		SlotContextAction UISlotEditorWidget<T>::DrawPriorityContextMenu(
			T                a_handle,
			entrySlotData_t& a_data)
		{
			SlotContextAction result{ SlotContextAction::None };

			ImGui::PushID("context_area");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			if (ImGui::BeginPopup("context_menu"))
			{
				if (a_data.priority)
				{
					if (ImGui::MenuItem(
							UIL::LS(CommonStrings::Clear, "1"),
							nullptr,
							false,
							a_data.priority->first == GetConfigClass()))
					{
						a_data.priority.reset();

						OnPriorityConfigClear(a_handle, { a_data });

						result = SlotContextAction::Delete;
					}
				}
				else
				{
					if (ImGui::MenuItem(UIL::LS(CommonStrings::Create, "1")))
					{
						a_data.priority = std::make_unique<Data::configSlotHolderCopy_t::prio_data_type>();

						OnPriorityConfigChange(a_handle, { GetSex(), *a_data.priority });

						result = SlotContextAction::Add;
					}
				}

				ImGui::Separator();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Copy, "2"),
						nullptr,
						false,
						a_data.priority.get() != nullptr))
				{
					if (a_data.priority)
					{
						UIClipboard::Set(a_data.priority->second.get(GetSex()));
					}
				}

				auto clipData = UIClipboard::Get<Data::configSlotPriority_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						if (!a_data.priority)
						{
							a_data.priority = std::make_unique<Data::configSlotHolderCopy_t::prio_data_type>();
						}

						auto sex = GetSex();

						a_data.priority->second.get(sex) = *clipData;

						OnPriorityConfigChange(a_handle, { sex, *a_data.priority });

						result = SlotContextAction::Paste;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawPriority(
			T                a_handle,
			entrySlotData_t& a_data)
		{
			ImGui::PushID("prio");

			const auto ctxresult = DrawPriorityContextMenu(a_handle, a_data);

			switch (ctxresult)
			{
			case SlotContextAction::Add:
			case SlotContextAction::Paste:
				ImGui::SetNextItemOpen(true);
				break;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (this->TreeEx(
					"tree",
					false,
					"%s",
					UIL::LS(CommonStrings::Priority)))
			{
				ImGui::Spacing();

				if (auto& pdata = a_data.priority)
				{
					bool pvar = pdata->first != GetConfigClass();

					if (pvar)
					{
						ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
					}

					if (ShowConfigClassIndicator())
					{
						ImGui::Text("%s:", UIL::LS(UIWidgetCommonStrings::ConfigInUse));
						ImGui::SameLine();
						DrawConfigClassInUse(pdata->first);

						ImGui::Separator();
						ImGui::Spacing();
					}

					DrawPriorityEntry(a_handle, *pdata);

					if (pvar)
					{
						ImGui::PopStyleVar();
					}
				}
				else
				{
					ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorGreyed);
					ImGui::TextWrapped("%s", UIL::LS(UISlotEditorWidgetStrings::EmptyPrioMsg));
					ImGui::PopStyleColor();
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawPriorityEntry(
			T                                a_handle,
			entrySlotData_t::prio_data_type& a_data)
		{
			auto  sex  = GetSex();
			auto& data = a_data.second.get(sex);

			bool changed = false;

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			ImGui::PushID("0");

			std::uint32_t i = 0;

			for (auto it = data.order.begin(); it != data.order.end(); ++it)
			{
				ImGui::PushID(i);

				bool pvar = i >= data.limit;

				if (pvar)
				{
					ImGui::PushStyleVar(
						ImGuiStyleVar_Alpha,
						std::max(ImGui::GetStyle().Alpha - 0.33f, 0.33f));
				}

				auto swapdir = SwapDirection::None;

				if (ImGui::ArrowButton("0", ImGuiDir_Up))
				{
					swapdir = SwapDirection::Up;
				}

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

				if (ImGui::ArrowButton("1", ImGuiDir_Down))
				{
					swapdir = SwapDirection::Down;
				}

				if (swapdir != SwapDirection::None)
				{
					IterSwap(data.order, it, swapdir);
					changed = true;
				}

				if (it != data.order.end())
				{
					ImGui::SameLine();

					ImGui::Text("%u: %s", i, Data::GetObjectTypeName(*it));
				}

				if (pvar)
				{
					ImGui::PopStyleVar();
				}

				ImGui::PopID();

				i++;
			}

			ImGui::PopID();

			ImGui::PopStyleVar();

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			changed |= ImGui::CheckboxFlagsT(
				UIL::LS(UISlotEditorWidgetStrings::AccountForEquipped, "1"),
				stl::underlying(std::addressof(data.flags.value)),
				stl::underlying(Data::SlotPriorityFlags::kAccountForEquipped));

			ImGui::Spacing();

			ImGui::PushItemWidth(ImGui::GetFontSize() * 5.5f);

			constexpr std::uint32_t lmin = 0;
			constexpr std::uint32_t lmax = stl::underlying(Data::ObjectType::kMax);

			changed |= ImGui::SliderScalar(
				UIL::LS(UISlotEditorWidgetStrings::MaxActiveTypes, "2"),
				ImGuiDataType_U32,
				std::addressof(data.limit),
				std::addressof(lmin),
				std::addressof(lmax),
				"%u",
				ImGuiSliderFlags_AlwaysClamp);

			ImGui::PopItemWidth();

			if (changed)
			{
				OnPriorityConfigChange(a_handle, { sex, a_data });
			}
		}

		template <class T>
		void UISlotEditorWidget<T>::EditorDrawMenuBarItems()
		{
			auto sex = GetSex();

			auto current = GetCurrentData();

			bool disabled = !current;

			UICommon::PushDisabled(disabled);

			if (UIL::LCG_MI(UIWidgetCommonStrings::CopyAllFromOppositeSex, "1"))
			{
				auto& queue = GetPopupQueue();

				queue.push(
						 UIPopupType::Confirm,
						 UIL::LS(CommonStrings::Confirm),
						 "%s",
						 UIL::LS(UIWidgetCommonStrings::CopyAllFromOppositeSexPrompt))
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
				if (UIL::LCG_MI(UISlotEditorWidgetStrings::ClearAllSlots, "2"))
				{
					auto& queue = GetPopupQueue();

					queue.push(
							 UIPopupType::Confirm,
							 UIL::LS(CommonStrings::Confirm),
							 "%s",
							 UIL::LS(UISlotEditorWidgetStrings::ClearAllSlotsPrompt))
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

			if (UIL::LCG_BM(UISlotEditorWidgetStrings::AllSlots, "3"))
			{
				if (UIL::LCG_MI(CommonStrings::Enable, "1"))
				{
					QueueSetAllSlotsEnabled(current.handle, sex, true);
				}

				if (UIL::LCG_MI(CommonStrings::Disable, "2"))
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
			T                a_handle,
			Data::ConfigSex  a_tsex,
			Data::ObjectSlot a_slot)
		{
			auto& queue = GetPopupQueue();

			queue.push(
					 UIPopupType::Confirm,
					 UIL::LS(CommonStrings::Confirm),
					 "%s",
					 UIL::LS(UISlotEditorWidgetStrings::CopyOppositeSexSlotPrompt))
				.call([this,
			           handle = a_handle,
			           slot   = a_slot,
			           tsex   = a_tsex,
			           ssex   = GetOppositeSex(a_tsex)](
						  const UIPopupAction&) {
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

					this->OnBaseConfigChange(
						handle,
						std::addressof(params),
						PostChangeAction::Reset);
				});
		}

		template <class T>
		void UISlotEditorWidget<T>::QueueClearSlot(
			T                a_handle,
			Data::ConfigSex  a_sex,
			Data::ObjectSlot a_slot)
		{
			auto& queue = GetPopupQueue();

			queue.push(
					 UIPopupType::Confirm,
					 UIL::LS(CommonStrings::Confirm),
					 "%s",
					 UIL::LS(UISlotEditorWidgetStrings::ClearSlotPrompt))
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
			T               a_handle,
			Data::ConfigSex a_sex,
			bool            a_switch)
		{
			auto& queue = GetPopupQueue();

			queue.push(
					 UIPopupType::Confirm,
					 UIL::LS(CommonStrings::Confirm),
					 "%s",
					 a_switch ?
						 UIL::LS(UISlotEditorWidgetStrings::EnableAllSlotsPrompt) :
						 UIL::LS(UISlotEditorWidgetStrings::DisableAllSlotsPrompt))
				.call([this, a_handle, a_sex, a_switch](
						  const UIPopupAction&) {
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
							data.flags.clear(Data::BaseFlags::kDisabled);
						}
						else
						{
							data.flags.set(Data::BaseFlags::kDisabled);
						}

						SingleSlotConfigUpdateParams params{ slot, a_sex, *slotData };

						this->OnBaseConfigChange(
							a_handle,
							std::addressof(params),
							PostChangeAction::Evaluate);
					}
				});
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotContextMenu(
			T                                   a_handle,
			const SingleSlotConfigUpdateParams& a_params)
		{
			if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::CopyFromOppositeSex, "1")))
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
					if (ImGui::MenuItem(UIL::LS(UISlotEditorWidgetStrings::ClearSlot, "2")))
					{
						QueueClearSlot(a_handle, a_params.sex, a_params.slot);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "3")))
			{
				UIClipboard::Set(a_params.entry.second.get(a_params.sex));
			}

			auto clipData = UIClipboard::Get<Data::configSlot_t>();

			if (ImGui::MenuItem(
					UIL::LS(CommonStrings::Paste, "4"),
					nullptr,
					false,
					clipData != nullptr))
			{
				if (clipData)
				{
					a_params.entry.second.get(a_params.sex) = *clipData;

					this->OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Reset);
				}
			}
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotHeaderControls(
			T                                   a_handle,
			const SingleSlotConfigUpdateParams& a_params)
		{
			ImGui::PushID("header_controls");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			if (ImGui::BeginPopup("context_menu"))
			{
				DrawSlotContextMenu(a_handle, a_params);
				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawSlotEntry(
			T                           a_handle,
			Data::ObjectSlot            a_slot,
			entrySlotData_t::data_type& a_entry)
		{
			SingleSlotConfigUpdateParams params{ a_slot, GetSex(), a_entry };

			DrawSlotHeaderControls(a_handle, params);

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			auto& slotName = StringHolder::GetSingleton().GetSlotName(a_slot);

			if (this->CollapsingHeader(
					"tree_slot",
					stl::underlying(a_slot) == 0,
					"%s",
					slotName.c_str()))
			{
				ImGui::PushID("body");

				ImGui::Spacing();

				ImGui::Indent();

				bool infoDrawn = ShowConfigClassIndicator();

				if (infoDrawn)
				{
					ImGui::Text("%s:", UIL::LS(UIWidgetCommonStrings::ConfigInUse));
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
						UIL::LS(UISlotEditorWidgetStrings::Left2HSlotWarning));
					ImGui::PopStyleColor();

					ImGui::Separator();
					ImGui::Spacing();
				}

				auto& data = params.entry.second.get(params.sex);
				auto& sh   = StringHolder::GetSingleton();

				ImGui::PushID("base_config");

				this->DrawBaseConfig(
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

				ImGui::PopID();
			}
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawMainHeaderControlsExtra(
			T                a_handle,
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
			T                             a_handle,
			SingleSlotConfigUpdateParams& a_params)
		{
			auto& data = a_params.entry.second.get(a_params.sex);

			const bool disabled = data.flags.test(Data::BaseFlags::kDisabled) &&
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
				UIL::LS(UIWidgetCommonStrings::ItemFilters),
				ffparams,
				data.itemFilter);

			ImGui::PopID();

			ImGui::PushID("fcond_tree");

			this->DrawEquipmentOverrideConditionTree(
				data.itemFilterCondition,
				[&] {
					this->OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Evaluate);
				},
				static_cast<Localization::StringID>(UIWidgetCommonStrings::ItemFilterCond));

			ImGui::PopID();

			UICommon::PopDisabled(disabled);
		}

		template <class T>
		void UISlotEditorWidget<T>::DrawPreferredItemsTree(
			T                             a_handle,
			SingleSlotConfigUpdateParams& a_params,
			Data::configSlot_t&           a_data)
		{
			const auto result = DrawPreferredItemsHeaderContextMenu(
				a_handle,
				a_params,
				a_data.preferredItems);

			const bool empty = a_data.preferredItems.empty();

			UICommon::PushDisabled(empty);

			if (!empty)
			{
				if (result.action == SlotContextAction::Add ||
				    result.action == SlotContextAction::Paste)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			if (this->TreeEx(
					"pi_tree",
					false,
					"%s",
					UIL::LS(UIWidgetCommonStrings::PreferredItems)))
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
			T                             a_handle,
			SingleSlotConfigUpdateParams& a_params,
			Data::configFormList_t&       a_data)
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
				auto w =
					(ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 3.0f + 2.0f;

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::FormID), ImGuiTableColumnFlags_None, 75.0f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Info), ImGuiTableColumnFlags_None, 250.0f);

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
					case SlotContextAction::Add:
						it = a_data.emplace(it, result.form);

						this->OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);

						ImGui::SetNextItemOpen(true);

						break;
					case SlotContextAction::Delete:
						it = a_data.erase(it);

						this->OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);

						break;
					case SlotContextAction::Swap:
						if (IterSwap(a_data, it, result.dir))
						{
							this->OnBaseConfigChange(
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
			T                             a_handle,
			SingleSlotConfigUpdateParams& a_params,
			Game::FormID&                 a_out)
		{
			if (ImGui::BeginPopup("form_edit_context_menu"))
			{
				if (m_formSelector.DrawFormSelector(
						m_piEditEntryID))
				{
					if (m_piEditEntryID)
					{
						a_out = m_piEditEntryID;
						this->OnBaseConfigChange(
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
			if (auto formInfo = this->LookupForm(a_form))
			{
				if (auto typeDesc = form_type_to_desc(formInfo->form.type))
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

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_piNewEntryID = {};
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("up", ImGuiDir_Up))
			{
				result.action = SlotContextAction::Swap;
				result.dir    = SwapDirection::Up;
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::ArrowButton("down", ImGuiDir_Down))
			{
				result.action = SlotContextAction::Swap;
				result.dir    = SwapDirection::Down;
			}

			ImGui::PopStyleVar();

			//ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Insert, "1"))
				{
					if (m_formSelector.DrawFormSelector(
							m_piNewEntryID))
					{
						if (m_piNewEntryID)
						{
							result.action = SlotContextAction::Add;
							result.form   = m_piNewEntryID;
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_MI(CommonStrings::Delete, "2"))
				{
					result.action = SlotContextAction::Delete;
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		FormEntryActionResult UISlotEditorWidget<T>::DrawPreferredItemsHeaderContextMenu(
			T                             a_handle,
			SingleSlotConfigUpdateParams& a_params,
			Data::configFormList_t&       a_data)
		{
			FormEntryActionResult result;

			ImGui::PushID("pi_header_context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_piNewEntryID = 0;
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::Add, "1"))
				{
					if (m_formSelector.DrawFormSelector(
							m_piNewEntryID))
					{
						a_data.emplace_back(m_piNewEntryID);

						this->OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);

						ImGui::CloseCurrentPopup();

						result.action = SlotContextAction::Add;
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "2")))
				{
					UIClipboard::Set(a_data);
				}

				auto clipData = UIClipboard::Get<Data::configFormList_t>();

				if (ImGui::MenuItem(
						UIL::LS(CommonStrings::Paste, "3"),
						nullptr,
						false,
						clipData != nullptr))
				{
					if (clipData)
					{
						a_data = *clipData;

						this->OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);

						result.action = SlotContextAction::Paste;
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
			if (this->TreeEx(
					"slot_filter",
					false,
					"%s",
					UIL::LS(CommonStrings::Filter)))
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