#pragma once

#include "IED/UI/PopupQueue/UIPopupAction.h"
#include "IED/UI/PopupQueue/UIPopupQueue.h"
#include "IED/UI/UIAllowedModelTypes.h"
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
#include "IED/UI/Widgets/UILastEquippedWidget.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UIVariableSourceSelectorWidget.h"
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
			public UIBaseConfigWidget<T>,
			public UIModelGroupEditorWidget<T>,
			public UILastEquippedWidget,
			public UIEditorPanelSettingsGear,
			public virtual UIFormTypeSelectorWidget,
			public UIEditorInterface
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

			virtual void EditorDrawMenuBarItems() override;

			virtual void DrawMenuBarItemsExtra();

			virtual void DrawItemContextMenu(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params);

			virtual constexpr Data::ConfigClass GetConfigClass() const = 0;

		private:
			void DrawFormSelectors(
				T                               a_handle,
				SingleCustomConfigUpdateParams& a_params);

			void DrawVariableModePanel(
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
			UIBaseConfigWidget<T>(a_controller),
			UIModelGroupEditorWidget<T>(m_formPicker, a_controller),
			UILastEquippedWidget(a_controller),
			m_itemFilter(true),
			m_formPicker(a_controller, FormInfoFlags::kValidCustom, true, true),
			m_controller(a_controller)
		{
			m_formPicker.SetAllowedTypes(g_allowedModelTypes);
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
				sorted.reserve(a_data.data.size());

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
						return stl::fixed_string::less_str{}(a_lhs->first, a_rhs->first);
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
					 UIL::LS(UIWidgetCommonStrings::NewItem),
					 "%s",
					 UIL::LS(UIWidgetCommonStrings::NewItemPrompt))
				.call([this](const auto& a_p) {
					auto& name = a_p.GetInput();

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
						UIL::LS(CommonStrings::Error),
						"%s",
						UIL::LS(UIWidgetCommonStrings::ItemAlreadyExists));
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
					 UIL::LS(CommonStrings::Confirm),
					 "%s",
					 UIL::LS(UIWidgetCommonStrings::CopyFromOppositeSexPrompt))
				.call([this,
			           handle = a_handle,
			           name   = a_data.name,
			           tsex   = a_data.sex,
			           ssex   = GetOppositeSex(a_data.sex)](
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

					this->OnBaseConfigChange(
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
					 UIL::LS(CommonStrings::Confirm),
					 "%s",
					 UIL::LS(UIWidgetCommonStrings::DeleteItemPrompt))
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
					 UIL::LS(UIWidgetCommonStrings::NewItem),
					 "%s",
					 UIL::LS(UIWidgetCommonStrings::RenamePrompt))
				.set_input(*a_data.name)
				.call([this, oldName = a_data.name](const auto& a_p) {
					stl::fixed_string newName(a_p.GetInput());

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
						UIL::LS(CommonStrings::Error),
						"%s",
						UIL::LS(UIWidgetCommonStrings::ItemAlreadyExists));
				});
		}

		template <class T>
		void UICustomEditorWidget<T>::OnPasteFail(
			const Data::configCustomNameValue_t* a_data)
		{
			auto& queue = GetPopupQueue();
			queue.push(
				UIPopupType::Message,
				UIL::LS(CommonStrings::Error),
				"%s",
				UIL::LS(UIWidgetCommonStrings::ItemAlreadyExists));
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
		void UICustomEditorWidget<T>::EditorDrawMenuBarItems()
		{
			bool disabled = !GetCurrentData();

			UICommon::PushDisabled(disabled);

			if (ImGui::MenuItem(UIL::LS(CommonStrings::New, "1")))
			{
				QueueAddItemPopup();
			}

			auto clipData = UIClipboard::Get<Data::configCustomNameValue_t>();

			if (ImGui::MenuItem(
					UIL::LS(CommonStrings::Paste, "2"),
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
			if (ImGui::MenuItem(UIL::LS(CommonStrings::Delete, "1")))
			{
				QueueErasePopup(a_handle, a_params);
			}

			if (ImGui::MenuItem(UIL::LS(CommonStrings::Rename, "2")))
			{
				QueueRenamePopup(a_handle, a_params);
			}

			ImGui::Separator();

			if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::CopyFromOppositeSex, "3")))
			{
				QueueCopySlotSexPopup(
					a_handle,
					a_params);
			}

			ImGui::Separator();

			if (ImGui::MenuItem(UIL::LS(CommonStrings::Copy, "4")))
			{
				UIClipboard::Set<Data::configCustomNameValue_t>(
					a_params.name,
					a_params.sex,
					a_params.entry);
			}

			auto clipData = UIClipboard::Get<Data::configCustomNameValue_t>();

			if (ImGui::MenuItem(
					UIL::LS(CommonStrings::PasteOver, "5"),
					nullptr,
					false,
					clipData != nullptr))
			{
				if (clipData)
				{
					a_params.entry(a_params.sex) = clipData->data(a_params.sex);

					this->OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Reset);
				}
			}
		}

		enum class CustomObjectMode
		{
			kSingle,
			kGroup,
			kLastEquipped
		};

		constexpr CustomObjectMode GetCustomObjectModel(
			const stl::flag<Data::CustomFlags>& a_flags) noexcept
		{
			if (a_flags.test(Data::CustomFlags::kLastEquippedMode))
			{
				return CustomObjectMode::kLastEquipped;
			}
			if (a_flags.test(Data::CustomFlags::kGroupMode))
			{
				return CustomObjectMode::kGroup;
			}
			return CustomObjectMode::kSingle;
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawFormSelectors(
			T                               a_handle,
			SingleCustomConfigUpdateParams& a_params)
		{
			auto& data = a_params.entry(a_params.sex);

			ImGui::PushID("fsel");

			ImGui::PushID("gs");

			auto mode = GetCustomObjectModel(data.customFlags);

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Single, "1"),
					mode == CustomObjectMode::kSingle))
			{
				data.customFlags.clear(Data::CustomFlags::kNonSingleMask);

				this->OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Reset);
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Group, "2"),
					mode == CustomObjectMode::kGroup))
			{
				data.customFlags.clear(Data::CustomFlags::kNonSingleMask);
				data.customFlags.set(Data::CustomFlags::kGroupMode);

				this->CreatePrimaryModelGroup(a_handle, a_params, data.group);

				this->OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Reset);
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					UIL::LS(UIWidgetCommonStrings::LastEquipped, "3"),
					mode == CustomObjectMode::kLastEquipped))
			{
				data.customFlags.clear(Data::CustomFlags::kNonSingleMask);
				data.customFlags.set(Data::CustomFlags::kLastEquippedMode);

				this->OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Reset);
			}

			ImGui::SameLine();

			UITipsInterface::DrawTip(UITip::CustomLastEquipped);

			ImGui::PopID();

			switch (mode)
			{
			case CustomObjectMode::kGroup:

				this->DrawModelGroupEditorWidgetTree(a_handle, a_params, data.group);

				break;

			case CustomObjectMode::kLastEquipped:

				DrawLastEquippedPanel(data.lastEquipped, [&] {
					this->OnBaseConfigChange(
						a_handle,
						std::addressof(a_params),
						PostChangeAction::Evaluate);
				});

				[[fallthrough]];
			default:

				if (!data.customFlags.test_any(Data::CustomFlags::kIsInInventoryMask))
				{
					if (ImGui::CheckboxFlagsT(
							UIL::LS(UICustomEditorString::UseFormVariable, "4"),
							stl::underlying(std::addressof(data.customFlags.value)),
							stl::underlying(Data::CustomFlags::kVariableMode)))
					{
						this->OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);
					}

					if (data.customFlags.test(Data::CustomFlags::kVariableMode))
					{
						ImGui::Indent();

						DrawVariableModePanel(a_handle, a_params);

						ImGui::Unindent();
					}
				}

				if (!(!data.customFlags.test_any(Data::CustomFlags::kIsInInventoryMask) &&
				      data.customFlags.test(Data::CustomFlags::kVariableMode)))
				{
					if (m_formPicker.DrawFormPicker(
							"fp",
							mode == CustomObjectMode::kLastEquipped ?
								static_cast<Localization::StringID>(CommonStrings::Default) :
								static_cast<Localization::StringID>(CommonStrings::Item),
							data.form,
							UITipsInterface::GetTipText(
								mode == CustomObjectMode::kLastEquipped ?
									UITip::CustomFormLastEquipped :
									UITip::CustomForm)))
					{
						this->OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);
					}
				}

				break;
			}

			ImGui::PopID();
		}

		template <class T>
		void UICustomEditorWidget<T>::DrawVariableModePanel(
			T                               a_handle,
			SingleCustomConfigUpdateParams& a_params)
		{
			auto& data = a_params.entry(a_params.sex);

			ImGui::PushID("vm_panel");

			if (UIVariableSourceSelectorWidget::DrawVariableSourceSelectorWidget(data.varSource.source))
			{
				this->OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Evaluate);
			}

			switch (data.varSource.source)
			{
			case Data::VariableSource::kActor:
				{
					auto& fp = m_condParamEditor.GetFormPicker();

					fp.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
					fp.SetFormBrowserEnabled(false);

					if (fp.DrawFormPicker(
							"ctl_1",
							static_cast<Localization::StringID>(CommonStrings::Form),
							data.varSource.form))
					{
						this->OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);
					}
				}
				break;
			case Data::VariableSource::kPlayerHorse:

				break;
			}

			ImGui::Spacing();

			if (this->DrawStringListTree(
					"ctl_2",
					static_cast<Localization::StringID>(CommonStrings::Variables),
					data.formVars,
					ImGuiTreeNodeFlags_SpanAvailWidth |
						ImGuiTreeNodeFlags_DefaultOpen))
			{
				this->OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Evaluate);
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
					UIL::LS(CommonStrings::Item)))
			{
				ImGui::Spacing();

				const bool disabled = data.flags.test(Data::BaseFlags::kDisabled) &&
				                      data.equipmentOverrides.empty();

				UICommon::PushDisabled(disabled);

				DrawFormSelectors(a_handle, a_params);

				ImGui::Spacing();

				if (ImGui::TreeNodeEx(
						"cond_tree",
						ImGuiTreeNodeFlags_DefaultOpen |
							ImGuiTreeNodeFlags_SpanAvailWidth,
						"%s",
						UIL::LS(CommonStrings::Conditions)))
				{
					ImGui::Spacing();

					ImGui::BeginGroup();

					if (ImGui::CheckboxFlagsT(
							UIL::LS(CommonStrings::Chance, "1"),
							stl::underlying(std::addressof(data.customFlags.value)),
							stl::underlying(Data::CustomFlags::kUseChance)))
					{
						this->OnBaseConfigChange(
							a_handle,
							std::addressof(a_params),
							PostChangeAction::Evaluate);
					}

					if (data.customFlags.test(Data::CustomFlags::kUseChance))
					{
						ImGui::SameLine();

						if (ImGui::SliderFloat(
								UIL::LS(CommonStrings::Percent, "2"),
								std::addressof(data.probability),
								0.0f,
								100.0f,
								"%.2f",
								ImGuiSliderFlags_AlwaysClamp))
						{
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
					}

					ImGui::EndGroup();

					UITipsInterface::DrawTip(UITip::CustomChance);

					bool cd = data.customFlags.test(Data::CustomFlags::kLastEquippedMode);

					UICommon::PushDisabled(cd);

					if (cd)
					{
						bool dummy = true;
						ImGui::Checkbox(
							UIL::LS(UICustomEditorString::IsInventoryItem, "3"),
							std::addressof(dummy));
					}
					else
					{
						if (ImGui::CheckboxFlagsT(
								UIL::LS(UICustomEditorString::IsInventoryItem, "3"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kIsInInventory)))
						{
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
					}

					UICommon::PopDisabled(cd);

					UITipsInterface::DrawTip(UITip::CustomInventoryItem);

					if (data.customFlags.test_any(Data::CustomFlags::kIsInInventoryMask))
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
									UIL::LS(UICustomEditorString::SelectedItemNotInventory));
							}
						}

						ImGui::Spacing();

						DrawExtraItems(a_handle, a_params);

						ImGui::Spacing();

						ImGui::Columns(2, nullptr, false);

						/*cd = data.customFlags.test(Data::CustomFlags::kUseLastEquipped);

						UICommon::PushDisabled(cd);

						if (cd)
						{
							bool dummy = true;
							ImGui::Checkbox(
								UILI::LS(UICustomEditorString::EquipmentMode, "4"),
								std::addressof(dummy));
						}
						else
						{*/
						if (ImGui::CheckboxFlagsT(
								UIL::LS(UICustomEditorString::EquipmentMode, "4"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kEquipmentMode)))
						{
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
						/*}

						UICommon::PopDisabled(cd);*/

						UITipsInterface::DrawTip(UITip::CustomEquipmentMode);

						cd = !data.customFlags.test(Data::CustomFlags::kEquipmentMode);

						UICommon::PushDisabled(cd);

						if (ImGui::CheckboxFlagsT(
								UIL::LS(UIWidgetCommonStrings::AlwaysUnload, "5"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kAlwaysUnload)))
						{
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}

						UICommon::PopDisabled(cd);

						UITipsInterface::DrawTip(UITip::AlwaysUnloadCustom);

						if (ImGui::CheckboxFlagsT(
								UIL::LS(UIWidgetCommonStrings::IsFavorited, "6"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kCheckFav)))
						{
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
						UITipsInterface::DrawTip(UITip::IsFavorited);

						ImGui::NextColumn();

						cd = !data.customFlags.test_any(Data::CustomFlags::kEquipmentModeMask);

						UICommon::PushDisabled(cd);

						if (ImGui::CheckboxFlagsT(
								UIL::LS(UICustomEditorString::IgnoreRaceEquipTypes, "8"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kIgnoreRaceEquipTypes)))
						{
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}

						UITipsInterface::DrawTip(UITip::IgnoreRaceEquipTypes);

						if (ImGui::CheckboxFlagsT(
								UIL::LS(UICustomEditorString::DisableIfEquipped, "9"),
								stl::underlying(std::addressof(data.customFlags.value)),
								stl::underlying(Data::CustomFlags::kDisableIfEquipped)))
						{
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}

						UITipsInterface::DrawTip(UITip::DisableIfEquipped);

						UICommon::PopDisabled(cd);

						ImGui::Columns();

						ImGui::Spacing();

						ImGui::PushItemWidth(ImGui::GetFontSize() * -13.5f);

						cd = data.customFlags.test(Data::CustomFlags::kGroupMode);

						UICommon::PushDisabled(cd);

						if (m_formPicker.DrawFormPicker(
								"fp_m",
								static_cast<Localization::StringID>(UICustomEditorString::ModelSwap),
								data.modelForm,
								UITipsInterface::GetTipText(UITip::CustomFormModelSwap)))
						{
							this->OnBaseConfigChange(
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
								UIL::LS(CommonStrings::Limits, "A"),
								ImGuiDataType_U32,
								std::addressof(data.countRange),
								2,
								0.175f,
								std::addressof(lim.min),
								std::addressof(lim.max),
								"%u",
								ImGuiSliderFlags_AlwaysClamp))
						{
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);
						}
						ImGui::EndGroup();

						UITipsInterface::DrawTip(UITip::CustomCountRange);

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

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				m_fsNew = {};
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(UIWidgetCommonStrings::AddOne, "1"))
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

							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);

							result = ExtraItemsAction::Add;

							m_fsNew = {};
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_BM(UIWidgetCommonStrings::AddMultiple, "2"))
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
							this->OnBaseConfigChange(
								a_handle,
								std::addressof(a_params),
								PostChangeAction::Evaluate);

							result = ExtraItemsAction::Add;
						}

						ImGui::CloseCurrentPopup();
					}

					ImGui::EndMenu();
				}

				if (UIL::LCG_MI(UIWidgetCommonStrings::ClearAll, "3"))
				{
					auto& data = a_params.entry(a_params.sex);

					data.extraItems.clear();

					this->OnBaseConfigChange(
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

			bool disabled = data.customFlags.test_any(Data::CustomFlags::kGroupMode);

			UICommon::PushDisabled(disabled);

			const auto result = DrawExtraItemsHeaderContextMenu(a_handle, a_params);

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
					UIL::LS(UIWidgetCommonStrings::AdditionalItems)))
			{
				if (!treeDisabled)
				{
					ImGui::Spacing();

					if (ImGui::CheckboxFlagsT(
							UIL::LS(UIWidgetCommonStrings::SelectRandomForm, "hctl_1"),
							stl::underlying(std::addressof(data.customFlags.value)),
							stl::underlying(Data::CustomFlags::kSelectInvRandom)))
					{
						this->OnBaseConfigChange(
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
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::FormID), ImGuiTableColumnFlags_None, 75.0f);
				ImGui::TableSetupColumn(UIL::LS(CommonStrings::Info), ImGuiTableColumnFlags_None, 250.0f);

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

						this->OnBaseConfigChange(
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
								this->OnBaseConfigChange(
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
								this->OnBaseConfigChange(
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

						if (auto formInfo = this->LookupForm(*it))
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

			ImGui::PushID("content");

			if (ImGui::TreeNodeEx(
					"tree_entry",
					ImGuiTreeNodeFlags_CollapsingHeader,
					"%s",
					a_name.c_str()))
			{
				ImGui::PushID("body");

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

				this->DrawBaseConfig(
					a_handle,
					data,
					std::addressof(params),
					a_name);

				ImGui::PopID();

				ImGui::Unindent();

				ImGui::Spacing();

				ImGui::PopID();
			}

			ImGui::PopID();
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
				if (ImGui::MenuItem(UIL::LS(UIWidgetCommonStrings::ResetToZero, "1")))
				{
					a_params.entry(a_params.sex).countRange = {};

					this->OnBaseConfigChange(
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

			if (this->Tree(UIL::LS(CommonStrings::Filter), false))
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
			ImGui::Columns(2, nullptr, false);

			auto params = static_cast<const SingleCustomConfigUpdateParams*>(a_params);

			auto& data = params->entry(params->sex);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::IgnorePlayer, "1"),
					stl::underlying(std::addressof(data.customFlags.value)),
					stl::underlying(Data::CustomFlags::kIgnorePlayer)))
			{
				this->OnBaseConfigChange(a_handle, a_params, PostChangeAction::Evaluate);
			}

			ImGui::NextColumn();

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::LeftWeapon, "3"),
					stl::underlying(std::addressof(data.customFlags.value)),
					stl::underlying(Data::CustomFlags::kLeftWeapon)))
			{
				this->OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
			}
			UITipsInterface::DrawTip(UITip::LeftWeapon);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::DisableHavok, "4"),
					stl::underlying(std::addressof(data.customFlags.value)),
					stl::underlying(Data::CustomFlags::kDisableHavok)))
			{
				this->OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
			}
			UITipsInterface::DrawTip(UITip::DisableHavok);

			/*if (ImGui::CheckboxFlagsT(
					UILI::LS(UICustomEditorString::LoadARMA, "3"),
					stl::underlying(std::addressof(data.customFlags.value)),
					stl::underlying(Data::CustomFlags::kLoadARMA)))
			{
				OnBaseConfigChange(a_handle, a_params, PostChangeAction::Reset);
			}
			UITipsInterface::DrawTip(UITip::LoadARMA);*/

			ImGui::Columns();
		}

		template <class T>
		inline bool UICustomEditorWidget<T>::GetEnableEquipmentOverridePropagation()
		{
			return GetEditorPanelSettings().eoPropagation;
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

				this->OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::Reset);

				break;
			case ModelGroupEditorOnChangeEventType::Transform:

				this->OnBaseConfigChange(
					a_handle,
					std::addressof(a_params),
					PostChangeAction::UpdateTransform);

				break;
			}
		}

	}
}