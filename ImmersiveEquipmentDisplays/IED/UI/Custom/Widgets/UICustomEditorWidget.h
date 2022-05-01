#pragma once

#include "IED/UI/PopupQueue/UIPopupQueue.h"
#include "IED/UI/UIClipboard.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIEditorInterface.h"
#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/Widgets/Filters/UIGenericFilter.h"
#include "IED/UI/Widgets/Form/UIFormPickerWidget.h"
#include "IED/UI/Widgets/UIBaseConfigWidget.h"
#include "IED/UI/Widgets/UICurrentData.h"
#include "IED/UI/Widgets/UIEditorPanelSettingsGear.h"
#include "IED/UI/Widgets/UIFormTypeSelectorWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/GlobalProfileManager.h"
#include "IED/StringHolder.h"

#include "UICustomEditorStrings.h"
#include "UIModelGroupEditorWidget.h"

#include "UISingleCustomConfigUpdateParams.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		using entryCustomData_t = Data::configCustomHolder_t;

		template <class T>
		struct profileSelectorParamsCustom_t
		{
			T                  handle;
			entryCustomData_t& data;
		};

		struct CustomConfigUpdateParams
		{
			Data::configCustomHolder_t& data;
		};

		struct CustomConfigNewParams
		{
			stl::fixed_string          name;
			Data::configCustomEntry_t& entry;
		};

		struct CustomConfigEraseParams
		{
			stl::fixed_string name;
		};

		struct CustomConfigRenameParams
		{
			stl::fixed_string oldName;
			stl::fixed_string newName;
		};

		enum class ExtraItemsAction : std::uint32_t
		{
			None,
			Add
		};

		template <class T>
		class UICustomEditorWidget :
			public UIEditorPanelSettingsGear,
			public UIBaseConfigWidget<T>,
			public UIEditorInterface,
			public UIModelGroupEditorWidget<T>,
			public virtual UIFormTypeSelectorWidget
		{
		public:
			UICustomEditorWidget(
				Controller& a_controller);

			void DrawCustomEditor(
				T                  a_handle,
				entryCustomData_t& a_data);

		protected:
			using CustomEditorCurrentData = UICurrentData<T, entryCustomData_t>;

			void QueueAddItemPopup();

			void QueueCopySlotSexPopup(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_data);

			void QueueErasePopup(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_data);

			void QueueRenamePopup(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_data);

			void OnPasteFail(const Data::configCustomNameValue_t* a_data);
			void DoPaste();

			virtual void DrawMenuBarItems() override;

			virtual void DrawMenuBarItemsExtra();

			virtual void DrawItemContextMenu(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params);

			virtual constexpr Data::ConfigClass GetConfigClass() const = 0;

		private:
			void DrawFormSelectors(
				T                               a_handle,
				SingleCustomConfigUpdateParams& a_params);

			void DrawCustomConfig(
				T                               a_handle,
				SingleCustomConfigUpdateParams& a_params);

			ExtraItemsAction DrawExtraItemsHeaderContextMenu(
				T                               a_handle,
				SingleCustomConfigUpdateParams& a_params);

			void DrawExtraItems(
				T                               a_handle,
				SingleCustomConfigUpdateParams& a_params);

			void DrawExtraItemsTable(
				T                               a_handle,
				SingleCustomConfigUpdateParams& a_params);

			void DrawCustomEntry(
				T                          a_handle,
				const stl::fixed_string&   a_name,
				Data::configCustomEntry_t& a_entry);

			virtual void DrawMainHeaderControlsExtra(
				T                  a_handle,
				entryCustomData_t& a_data);

			void DrawItemHeaderControls(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params);

			void DrawCountRangeContextMenu(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params);

			void DrawItemFilter();

			virtual void DrawExtraFlags(
				T                         a_handle,
				Data::configBaseValues_t& a_data,
				Data::configBase_t*       a_baseConfig,
				const void*               a_params) override;

			virtual UIPopupQueue&           GetPopupQueue()  = 0;
			virtual CustomEditorCurrentData GetCurrentData() = 0;

			virtual bool GetEnableEquipmentOverridePropagation() override;

			virtual void OnFullConfigChange(
				T                               a_handle,
				const CustomConfigUpdateParams& a_params) = 0;

			virtual bool OnCreateNew(
				T                            a_handle,
				const CustomConfigNewParams& a_params) = 0;

			virtual void OnErase(
				T                              a_handle,
				const CustomConfigEraseParams& a_params) = 0;

			virtual bool OnRename(
				T                               a_handle,
				const CustomConfigRenameParams& a_params) = 0;

			virtual void DrawExtraEquipmentOverrideOptions(
				T                          a_handle,
				Data::configBase_t&        a_data,
				const void*                a_params,
				Data::equipmentOverride_t& a_override) override;

			virtual bool DrawExtraItemInfo(
				T                                a_handle,
				const stl::fixed_string&         a_name,
				const Data::configCustomEntry_t& a_entry,
				bool                             a_infoDrawn);

			virtual void OnModelGroupEditorChange(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params,
				ModelGroupEditorOnChangeEventType     a_type) override;

			UIGenericFilter m_itemFilter;

			UIFormPickerWidget m_formPicker;

			Game::FormID m_fsNew;

			Controller& m_controller;
		};

		template <class T>
		UICustomEditorWidget<T>::UICustomEditorWidget(
			Controller& a_controller) :
			UIEditorPanelSettingsGear(a_controller),
			UIBaseConfigWidget<T>(a_controller),
			UIModelGroupEditorWidget<T>(m_formPicker, a_controller),
			m_itemFilter(true),
			m_formPicker(a_controller, FormInfoFlags::kValidCustom, true, true),
			m_controller(a_controller)
		{
			m_formPicker.SetAllowedTypes(
				{ TESObjectWEAP::kTypeID,
			      TESObjectARMO::kTypeID,
			      TESAmmo::kTypeID,
			      TESObjectLIGH::kTypeID,
			      TESObjectMISC::kTypeID,
			      IngredientItem::kTypeID,
			      AlchemyItem::kTypeID,
			      TESKey::kTypeID,
			      TESObjectBOOK::kTypeID,
			      TESSoulGem::kTypeID,
			      ScrollItem::kTypeID,
			      TESObjectSTAT::kTypeID,
			      BGSMovableStatic::kTypeID,
			      TESObjectTREE::kTypeID,
			      TESFlora::kTypeID,
			      TESFurniture::kTypeID,
			      BGSArtObject::kTypeID,
			      TESObjectACTI::kTypeID,
			      BGSTalkingActivator::kTypeID,
			      TESGrass::kTypeID,
			      TESObjectANIO::kTypeID,
			      TESObjectDOOR::kTypeID,
			      BGSExplosion::kTypeID,
			      BGSMaterialObject::kTypeID,
			      IFormDatabase::EXTRA_TYPE_ARMOR });
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawCustomEditor(
			T                  a_handle,
			entryCustomData_t& a_data)
		{
			ImGui::PushID("custom_editor");

			DrawEditorPanelSettings();

			ImGui::Spacing();

			ImGui::PushID("extra_header");

			DrawMainHeaderControlsExtra(a_handle, a_data);

			ImGui::PopID();

			ImGui::Separator();

			DrawItemFilter();

			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::BeginChild("custom_editor_panel", { -1.0f, 0.0f }))
			{
				stl::vector<decltype(a_data.data)::value_type*> sorted;

				for (auto& e : a_data.data)
				{
					if (!m_itemFilter.Test(*e.first))
					{
						continue;
					}

					sorted.emplace_back(std::addressof(e));
				}

				std::sort(
					sorted.begin(),
					sorted.end(),
					[](const auto& a_lhs, const auto& a_rhs) {
						return a_lhs->first < a_rhs->first;
					});

				ImGui::PushItemWidth(ImGui::GetFontSize() * -10.5f);

				for (auto& e : sorted)
				{
					ImGui::PushID(e->first.c_str());

					DrawCustomEntry(a_handle, e->first, e->second);

					ImGui::PopID();

					ImGui::Spacing();
				}

				ImGui::PopItemWidth();
			}

			ImGui::EndChild();

			ImGui::PopID();
		}

		template <class T>
		void UICustomEditorWidget<T>::QueueAddItemPopup()
		{
			auto& queue = GetPopupQueue();

			queue.push(
					 UIPopupType::Input,
					 LS(UIWidgetCommonStrings::NewItem),
					 "%s",
					 LS(UIWidgetCommonStrings::NewItemPrompt))
				.call([this](const auto& a_p) {
					std::string name(a_p.GetInput());

					if (name.empty())
					{
						return;
					}

					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					auto r = current.data->data.try_emplace(name);

					if (r.second)
					{
						CustomConfigNewParams params{
							r.first->first,
							r.first->second
						};

						if (OnCreateNew(current.handle, params))
						{
							return;
						}

						current.data->data.erase(r.first);
					}

					GetPopupQueue().push(
						UIPopupType::Message,
						LS(CommonStrings::Error),
						"%s",
						LS(UIWidgetCommonStrings::ItemAlreadyExists));
				});
		}

		template <class T>
		void UICustomEditorWidget<T>::QueueCopySlotSexPopup(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_data)
		{
			auto& queue = GetPopupQueue();

			queue.push(
					 UIPopupType::Confirm,
					 LS(CommonStrings::Confirm),
					 "%s",
					 LS(UIWidgetCommonStrings::CopyFromOppositeSexPrompt))
				.call([this,
			           handle = a_handle,
			           name   = a_data.name,
			           tsex   = a_data.sex,
			           ssex   = GetOppositeSex(a_data.sex)](
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

					auto it = current.data->data.find(name);
					if (it == current.data->data.end())
					{
						return;
					}

					it->second(tsex) = it->second(ssex);

					SingleCustomConfigUpdateParams params{
						name,
						tsex,
						it->second
					};

					OnBaseConfigChange(
						handle,
						std::addressof(params),
						PostChangeAction::Reset);
				});
		}

		template <class T>
		inline void UICustomEditorWidget<T>::QueueErasePopup(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_data)
		{
			auto& queue = GetPopupQueue();

			queue.push(
					 UIPopupType::Confirm,
					 LS(CommonStrings::Confirm),
					 "%s",
					 LS(UIWidgetCommonStrings::DeleteItemPrompt))
				.call([this,
			           handle = a_handle,
			           name   = a_data.name](
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

					current.data->data.erase(name);

					OnErase(handle, { name });
				});
		}

		template <class T>
		inline void UICustomEditorWidget<T>::QueueRenamePopup(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_data)
		{
			auto& queue = GetPopupQueue();

			queue.push(
					 UIPopupType::Input,
					 LS(UIWidgetCommonStrings::NewItem),
					 "%s",
					 LS(UIWidgetCommonStrings::RenamePrompt))
				.fmt_input("%s", a_data.name.c_str())
				.call([this, oldName = a_data.name](const auto& a_p) {
					auto& in = a_p.GetInput();

					stl::fixed_string newName(in);

					if (newName.empty())
					{
						return;
					}

					auto current = GetCurrentData();
					if (!current)
					{
						return;
					}

					auto ito = current.data->data.find(oldName);
					if (ito == current.data->data.end())
					{
						return;
					}

					auto itn = current.data->data.find(newName);
					if (itn == current.data->data.end())
					{
						CustomConfigRenameParams params{
							oldName,
							newName
						};

						if (OnRename(current.handle, params))
						{
							auto tmp = ito->second;

							current.data->data.erase(ito);
							current.data->data.insert_or_assign(newName, tmp);

							return;
						}
					}

					GetPopupQueue().push(
						UIPopupType::Message,
						LS(CommonStrings::Error),
						"%s",
						LS(UIWidgetCommonStrings::ItemAlreadyExists));
				});
		}

		template <class T>
		void UICustomEditorWidget<T>::OnPasteFail(
			const Data::configCustomNameValue_t* a_data)
		{
			auto& queue = GetPopupQueue();
			queue.push(
				UIPopupType::Message,
				LS(CommonStrings::Error),
				"%s",
				LS(UIWidgetCommonStrings::ItemAlreadyExists));
		}

		template <class T>
		void UICustomEditorWidget<T>::DoPaste()
		{
			auto clipData = UIClipboard::Get<Data::configCustomNameValue_t>();

			if (!clipData)
			{
				return;
			}

			auto current = GetCurrentData();
			if (!current)
			{
				return;
			}

			auto name(clipData->name);

			for (std::uint32_t i = 1;; i++)
			{
				if (!current.data->data.contains(name))
				{
					break;
				}

				if (i == 10000)
				{
					OnPasteFail(clipData);
					return;
				}

				name = std::move((std::string(clipData->name) += " ") += std::to_string(i));
			}

			auto r = current.data->data.emplace(name, clipData->data);

			if (r.second)
			{
				CustomConfigNewParams params{
					r.first->first,
					r.first->second
				};

				if (!OnCreateNew(current.handle, params))
				{
					current.data->data.erase(r.first);
				}
				else
				{
					return;
				}
			}

			OnPasteFail(clipData);
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawMenuBarItems()
		{
			bool disabled = !GetCurrentData();

			UICommon::PushDisabled(disabled);

			if (ImGui::MenuItem(LS(CommonStrings::New, "1")))
			{
				QueueAddItemPopup();
			}

			auto clipData = UIClipboard::Get<Data::configCustomNameValue_t>();

			if (ImGui::MenuItem(
					LS(CommonStrings::Paste, "2"),
					nullptr,
					false,
					clipData != nullptr))
			{
				DoPaste();
			}

			UICommon::PopDisabled(disabled);

			DrawMenuBarItemsExtra();
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawMenuBarItemsExtra()
		{
		}

		template <class T>
		inline void UICustomEditorWidget<T>::DrawItemContextMenu(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params)
		{
			if (ImGui::MenuItem(LS(CommonStrings::Delete, "1")))
			{
				QueueErasePopup(a_handle, a_params);
			}

			if (ImGui::MenuItem(LS(CommonStrings::Rename, "2")))
			{
				QueueRenamePopup(a_handle, a_params);
			}

			ImGui::Separator();

			if (ImGui::MenuItem(LS(UIWidgetCommonStrings::CopyFromOppositeSex, "3")))
			{
				QueueCopySlotSexPopup(
					a_handle,
					a_params);
			}

			ImGui::Separator();

			if (ImGui::MenuItem(LS(CommonStrings::Copy, "4")))
			{
				UIClipboard::Set<Data::configCustomNameValue_t>(
					a_params.name,
					a_params.sex,
					a_params.entry);
			}

			auto clipData = UIClipboard::Get<Data::configCustomNameValue_t>();

			if (ImGui::MenuItem(
					LS(CommonStrings::PasteOver, "5"),
					nullptr,
					false,
					clipData != nullptr))
			{
				if (clipData)
				{
					a_params.entry(a_params.sex) = clipData->data(a_params.sex);

					OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Reset);
				}
			}
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawFormSelectors(
			T                               a_handle,
			SingleCustomConfigUpdateParams& a_params)
		{
			auto& data = a_params.entry(a_params.sex);

			ImGui::PushID("fsel");

			ImGui::PushID("gs");

			if (ImGui::RadioButton(
					LS(CommonStrings::Single, "1"),
					!data.customFlags.test(Data::CustomFlags::kUseGroup)))
			{
				data.customFlags.clear(Data::CustomFlags::kUseGroup);

				OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Reset);
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					LS(CommonStrings::Group, "2"),
					data.customFlags.test(Data::CustomFlags::kUseGroup)))
			{
				data.customFlags.set(Data::CustomFlags::kUseGroup);

				CreatePrimaryModelGroup(a_handle, a_params, data.group);

				OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Reset);
			}

			ImGui::PopID();

			if (data.customFlags.test(Data::CustomFlags::kUseGroup))
			{
				DrawModelGroupEditorWidgetTree(a_handle, a_params, data.group);
			}
			else
			{
				if (m_formPicker.DrawFormPicker(
						"fp",
						static_cast<Localization::StringID>(CommonStrings::Item),
						data.form,
						GetTipText(UITip::CustomForm)))
				{
					OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Evaluate);
				}
			}

			ImGui::PopID();
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawCustomConfig(
			T                               a_handle,
			SingleCustomConfigUpdateParams& a_params)
		{
			auto& data = a_params.entry(a_params.sex);

			ImGui::PushID("custom_item");

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Item)))
			{
				ImGui::Spacing();

				const bool disabled = data.flags.test(Data::BaseFlags::kDisabled) &&
				                      data.equipmentOverrides.empty();

				UICommon::PushDisabled(disabled);

				DrawFormSelectors(a_handle, a_params);

				if (ImGui::TreeNodeEx(
						"cond_tree",
						ImGuiTreeNodeFlags_DefaultOpen |
							ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						LS(CommonStrings::Conditions)))
				{
					ImGui::Spacing();

					ImGui::BeginGroup();

					if (ImGui::CheckboxFlagsT(
							LS(CommonStrings::Chance, "1"),
							stl::underlying(std::addressof(data.customFlags.value)),
							stl::underlying(Data::CustomFlags::kUseChance)))
					{
						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);
					}

					if (data.customFlags.test(Data::CustomFlags::kUseChance))
					{
						ImGui::SameLine();

						if (ImGui::SliderFloat(
								LS(CommonStrings::Percent, "2"),
								std::addressof(data.chance),
								0.0f,
								100.0f,
								"%.2f",
								ImGuiSliderFlags_AlwaysClamp))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
					}

					ImGui::EndGroup();

					DrawTip(UITip::CustomChance);

					if (ImGui::CheckboxFlagsT(
							LS(UICustomEditorString::IsInventoryItem, "3"),
							stl::underlying(std::addressof(data.customFlags.value)),
							stl::underlying(Data::CustomFlags::kIsInInventory)))
					{
						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);
					}
					DrawTip(UITip::CustomInventoryItem);

					if (data.customFlags.test(Data::CustomFlags::kIsInInventory))
					{
						ImGui::Indent();

						if (auto formInfo = m_formPicker.GetCurrentInfo())
						{
							if (!IFormCommon::IsInventoryFormType(formInfo->form.type))
							{
								ImGui::Spacing();
								ImGui::TextColored(
									UICommon::g_colorWarning,
									"%s",
									LS(UICustomEditorString::SelectedItemNotInventory));
							}
						}

						ImGui::Spacing();

						DrawExtraItems(a_handle, a_params);

						ImGui::Spacing();

						ImGui::Columns(2, nullptr, false);

						if (ImGui::CheckboxFlagsT(
								LS(UICustomEditorString::EquipmentMode, "4"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kEquipmentMode)))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
						DrawTip(UITip::CustomEquipmentMode);

						if (ImGui::CheckboxFlagsT(
								LS(UIWidgetCommonStrings::AlwaysUnload, "5"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kAlwaysUnload)))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
						DrawTip(UITip::AlwaysUnloadCustom);

						if (ImGui::CheckboxFlagsT(
								LS(UIWidgetCommonStrings::IsFavorited, "6"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kCheckFav)))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
						DrawTip(UITip::IsFavorited);

						ImGui::NextColumn();

						bool cd = !data.customFlags.test(Data::CustomFlags::kEquipmentMode);

						UICommon::PushDisabled(cd);

						if (ImGui::CheckboxFlagsT(
								LS(UICustomEditorString::IgnoreRaceEquipTypes, "8"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kIgnoreRaceEquipTypes)))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}

						DrawTip(UITip::IgnoreRaceEquipTypes);

						if (ImGui::CheckboxFlagsT(
								LS(UICustomEditorString::DisableIfEquipped, "9"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kDisableIfEquipped)))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}

						DrawTip(UITip::DisableIfEquipped);

						UICommon::PopDisabled(cd);

						ImGui::Columns();

						ImGui::Spacing();

						ImGui::PushItemWidth(ImGui::GetFontSize() * -13.5f);

						cd = data.customFlags.test(Data::CustomFlags::kUseGroup);

						UICommon::PushDisabled(cd);

						if (m_formPicker.DrawFormPicker(
								"fp_m",
								static_cast<Localization::StringID>(UICustomEditorString::ModelSwap),
								data.modelForm,
								GetTipText(UITip::CustomFormModelSwap)))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Reset);
						}

						UICommon::PopDisabled(cd);

						ImGui::BeginGroup();
						DrawCountRangeContextMenu(a_handle, a_params);

						Data::configRange_t lim{
							0,
							UINT32_MAX
						};

						if (ImGui::DragScalarN(
								LS(CommonStrings::Limits, "A"),
								ImGuiDataType_U32,
								std::addressof(data.countRange),
								2,
								0.175f,
								std::addressof(lim.min),
								std::addressof(lim.max),
								"%u",
								ImGuiSliderFlags_AlwaysClamp))
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
						ImGui::EndGroup();

						DrawTip(UITip::CustomCountRange);

						ImGui::PopItemWidth();

						ImGui::Unindent();
					}

					ImGui::TreePop();
				}

				UICommon::PopDisabled(disabled);

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <class T>
		ExtraItemsAction UICustomEditorWidget<T>::DrawExtraItemsHeaderContextMenu(
			T                               a_handle,
			SingleCustomConfigUpdateParams& a_params)
		{
			ExtraItemsAction result{ ExtraItemsAction ::None };

			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(UIWidgetCommonStrings::AddOne, "1"))
				{
					if (m_formPicker.DrawFormSelector(m_fsNew))
					{
						auto& data = a_params.entry(a_params.sex);

						if (std::find(
								data.extraItems.begin(),
								data.extraItems.end(),
								m_fsNew) == data.extraItems.end())
						{
							data.extraItems.emplace_back(m_fsNew);

							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);

							result = ExtraItemsAction::Add;

							m_fsNew = 0;
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (LCG_BM(UIWidgetCommonStrings::AddMultiple, "2"))
				{
					if (m_formPicker.DrawFormSelectorMulti())
					{
						auto& data = a_params.entry(a_params.sex);

						bool added = false;

						for (auto& e : m_formPicker.GetSelectedEntries().getvec())
						{
							if (std::find(
									data.extraItems.begin(),
									data.extraItems.end(),
									e->second.formid) == data.extraItems.end())
							{
								data.extraItems.emplace_back(e->second.formid);
								added = true;
							}
						}

						m_formPicker.ClearSelectedEntries();

						if (added)
						{
							OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);

							result = ExtraItemsAction::Add;
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (LCG_MI(UIWidgetCommonStrings::ClearAll, "3"))
				{
					auto& data = a_params.entry(a_params.sex);

					data.extraItems.clear();

					OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Evaluate);
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();

			return result;
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawExtraItems(
			T                               a_handle,
			SingleCustomConfigUpdateParams& a_params)
		{
			auto& data = a_params.entry(a_params.sex);

			ImGui::PushID("extra_items");

			bool disabled = data.customFlags.test(Data::CustomFlags::kUseGroup);

			UICommon::PushDisabled(disabled);

			auto result = DrawExtraItemsHeaderContextMenu(a_handle, a_params);

			bool treeDisabled = disabled || data.extraItems.empty();

			if (!treeDisabled && !disabled)
			{
				if (result == ExtraItemsAction::Add)
				{
					ImGui::SetNextItemOpen(true);
				}
			}

			UICommon::PushDisabled(treeDisabled);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(UIWidgetCommonStrings::AdditionalItems)))
			{
				if (!treeDisabled)
				{
					ImGui::Spacing();

					if (ImGui::CheckboxFlagsT(
							LS(UIWidgetCommonStrings::SelectRandomForm, "hctl_1"),
							stl::underlying(std::addressof(data.customFlags.value)),
							stl::underlying(Data::CustomFlags::kSelectInvRandom)))
					{
						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Reset);
					}

					ImGui::Spacing();

					DrawExtraItemsTable(a_handle, a_params);

					ImGui::Spacing();
				}

				ImGui::TreePop();
			}

			UICommon::PopDisabled(treeDisabled);

			UICommon::PopDisabled(disabled);

			ImGui::PopID();
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawExtraItemsTable(
			T                               a_handle,
			SingleCustomConfigUpdateParams& a_params)
		{
			auto& data = a_params.entry(a_params.sex);

			if (data.extraItems.empty())
			{
				return;
			}

			ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 4.f, 4.f });

			constexpr int NUM_COLUMNS = 3;

			if (ImGui::BeginTable(
					"table",
					NUM_COLUMNS,
					ImGuiTableFlags_Borders |
						ImGuiTableFlags_Resizable |
						ImGuiTableFlags_NoSavedSettings |
						ImGuiTableFlags_SizingStretchProp,
					{ -1.0f, 0.f }))
			{
				auto w =
					((ImGui::GetFontSize() + ImGui::GetStyle().ItemInnerSpacing.x) * 2.0f) + 2.0f +
					ImGui::CalcTextSize("X", nullptr, true).x + (4.0f * 2.0f + 2.0f);

				ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_WidthFixed, w);
				ImGui::TableSetupColumn(LS(CommonStrings::FormID), ImGuiTableColumnFlags_None, 75.0f);
				ImGui::TableSetupColumn(LS(CommonStrings::Info), ImGuiTableColumnFlags_None, 250.0f);

				ImGui::TableNextRow(ImGuiTableRowFlags_Headers);

				for (int column = 0; column < NUM_COLUMNS; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::TableHeader(ImGui::TableGetColumnName(column));
				}

				auto it = data.extraItems.begin();

				int i = 0;

				while (it != data.extraItems.end())
				{
					ImGui::PushID(i);

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

					if (ImGui::Button("X"))
					{
						it = data.extraItems.erase(it);

						OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);
					}

					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::ArrowButton("up", ImGuiDir_Up))
					{
						if (it != data.extraItems.end())
						{
							if (IterSwap(data.extraItems, it, SwapDirection::Up))
							{
								OnBaseConfigChange(
									a_handle,
									std::addressof(a_params),
									PostChangeAction::Evaluate);
							}
						}
					}

					ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

					if (ImGui::ArrowButton("down", ImGuiDir_Down))
					{
						if (it != data.extraItems.end())
						{
							if (IterSwap(data.extraItems, it, SwapDirection::Down))
							{
								OnBaseConfigChange(
									a_handle,
									std::addressof(a_params),
									PostChangeAction::Evaluate);
							}
						}
					}

					ImGui::PopStyleVar();

					if (it != data.extraItems.end())
					{
						ImGui::TableSetColumnIndex(1);

						ImGui::Text("%.8X", it->get());

						ImGui::TableSetColumnIndex(2);

						if (auto formInfo = LookupForm(*it))
						{
							if (auto typeDesc = form_type_to_desc(formInfo->form.type))
							{
								ImGui::Text("[%s] %s", typeDesc, formInfo->form.name.c_str());
							}
							else
							{
								ImGui::Text("[%hhu] %s", formInfo->form.type, formInfo->form.name.c_str());
							}
						}

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
		inline void UICustomEditorWidget<T>::DrawCustomEntry(
			T                          a_handle,
			const stl::fixed_string&   a_name,
			Data::configCustomEntry_t& a_entry)
		{
			SingleCustomConfigUpdateParams params{ a_name, GetSex(), a_entry };

			DrawItemHeaderControls(a_handle, params);

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::TreeNodeEx(
					"tree_entry",
					ImGuiTreeNodeFlags_CollapsingHeader,
					"%s",
					a_name.c_str()))
			{
				ImGui::Spacing();

				ImGui::Indent();

				ImGui::PushID("extra_item_info");

				bool infoDrawn = DrawExtraItemInfo(a_handle, a_name, a_entry, false);

				ImGui::PopID();

				if (infoDrawn)
				{
					ImGui::Spacing();
					ImGui::Separator();
					ImGui::Spacing();
				}

				auto& data = a_entry(params.sex);

				ImGui::PushID("custom_config");

				DrawCustomConfig(a_handle, params);

				ImGui::PopID();

				ImGui::Spacing();

				ImGui::PushID("base_config");

				DrawBaseConfig(
					a_handle,
					data,
					std::addressof(params),
					a_name);

				ImGui::PopID();

				ImGui::Unindent();

				ImGui::Spacing();
			}
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawMainHeaderControlsExtra(
			T                  a_handle,
			entryCustomData_t& a_data)
		{
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawItemHeaderControls(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params)
		{
			ImGui::PushID("header_controls");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			if (ImGui::BeginPopup("context_menu"))
			{
				DrawItemContextMenu(a_handle, a_params);
				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawCountRangeContextMenu(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params)
		{
			ImGui::PushID("cr_context_area");

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");
			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (ImGui::MenuItem(LS(UIWidgetCommonStrings::ResetToZero, "1")))
				{
					a_params.entry(a_params.sex).countRange = {};

					OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Evaluate);
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		template <class T>
		inline void UICustomEditorWidget<T>::DrawItemFilter()
		{
			ImGui::PushID("item_filter");

			if (Tree(LS(CommonStrings::Filter), false))
			{
				ImGui::PushItemWidth(ImGui::GetFontSize() * -8.0f);

				ImGui::Spacing();

				m_itemFilter.Draw();

				ImGui::Spacing();

				ImGui::PopItemWidth();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawExtraFlags(
			T                         a_handle,
			Data::configBaseValues_t& a_data,
			Data::configBase_t*       a_baseConfig,
			const void*               a_params)
		{
			ImGui::Separator();

			ImGui::Columns(2, nullptr, false);

			auto params = static_cast<const SingleCustomConfigUpdateParams*>(a_params);

			auto& data = params->entry(params->sex);

			if (ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::IgnorePlayer, "1"),
					stl::underlying(std::addressof(data.customFlags.value)),
					stl::underlying(Data::CustomFlags::kIgnorePlayer)))
			{
				OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
			}

			ImGui::NextColumn();

			if (ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::LeftWeapon, "3"),
					stl::underlying(std::addressof(data.customFlags.value)),
					stl::underlying(Data::CustomFlags::kLeftWeapon)))
			{
				OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
			}
			DrawTip(UITip::LeftWeapon);

			if (ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::DisableHavok, "4"),
					stl::underlying(std::addressof(data.customFlags.value)),
					stl::underlying(Data::CustomFlags::kDisableHavok)))
			{
				OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
			}
			DrawTipImportant(UITip::DisableHavok);

			/*if (ImGui::CheckboxFlagsT(
					LS(UICustomEditorString::LoadARMA, "3"),
					stl::underlying(std::addressof(data.customFlags.value)),
					stl::underlying(Data::CustomFlags::kLoadARMA)))
			{
				OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
			}
			DrawTip(UITip::LoadARMA);*/

			ImGui::Columns();
		}

		template <class T>
		inline bool UICustomEditorWidget<T>::GetEnableEquipmentOverridePropagation()
		{
			return GetEditorPanelSettings().eoPropagation;
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawExtraEquipmentOverrideOptions(
			T                          a_handle,
			Data::configBase_t&        a_data,
			const void*                a_params,
			Data::equipmentOverride_t& a_override)
		{
		}

		template <class T>
		bool UICustomEditorWidget<T>::DrawExtraItemInfo(
			T                                a_handle,
			const stl::fixed_string&         a_name,
			const Data::configCustomEntry_t& a_entry,
			bool                             a_infoDrawn)
		{
			return false;
		}

		template <class T>
		void UICustomEditorWidget<T>::OnModelGroupEditorChange(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params,
			ModelGroupEditorOnChangeEventType     a_type)
		{
			switch (a_type)
			{
			case ModelGroupEditorOnChangeEventType::Form:
			case ModelGroupEditorOnChangeEventType::Flags:

				OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Reset);

				break;
			case ModelGroupEditorOnChangeEventType::Transform:

				OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::UpdateTransform);

				break;
			}
		}

	}
}