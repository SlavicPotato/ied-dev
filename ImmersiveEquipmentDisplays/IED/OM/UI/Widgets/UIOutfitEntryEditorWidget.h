#pragma once

#include "IED/UI/Widgets/Form/UIFormPickerWidget.h"
#include "IED/UI/Widgets/UIEditorPanelSettings.h"
#include "IED/UI/Widgets/UIEquipmentOverrideWidget.h"

#include "IED/UI/UIEditorInterface.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"
#include "IED/UI/UIFormLookupInterface.h"
#include "IED/UI/UISettingsInterface.h"

#include "IED/OM/ConfigOutfitEntryHolder.h"
#include "IED/OM/ConfigOutfitOverride.h"

#include "IED/UI/UILocalizationInterface.h"

#include "IED/OM/PersistentOutfitFormManager.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		namespace OM
		{
			using entryOutfitData_t = Data::OM::configOutfitEntryHolder_t;

			template <class T>
			struct profileSelectorParamsOutfit_t
			{
				const T&           params;
				entryOutfitData_t& data;
			};

			template <class T>
			struct UIOutfitEditorParamsHandle
			{
				T            handle;
				mutable bool wantDim{ false };
			};

			/*template <class T>
			struct UIOutfitConfigEquipmentOverrideParams
			{
				const T& a_params;
			};*/

			template <class T>
			struct UIOutfitEntryChangeParams
			{
				const T&           params;
				Data::ConfigSex    sex;
				entryOutfitData_t& data;
			};

			template <class T>
			struct UIOutfitEntryEditorWidgetParamWrapper
			{
				const T& actual;
				bool     noData;
			};

			template <class _Param>
			class UIOutfitEntryEditorWidget :
				public UIEquipmentOverrideWidget<
					Data::OM::outfitOverrideList_t,
					UIOutfitEntryChangeParams<
						UIOutfitEntryEditorWidgetParamWrapper<_Param>>>,
				public UIEditorInterface,
				public UIFormLookupInterface,
				public UIEditorPanelSettings,
				public virtual UISettingsInterface
			{
			public:
				using change_param_type = UIOutfitEntryChangeParams<UIOutfitEntryEditorWidgetParamWrapper<_Param>>;

				UIOutfitEntryEditorWidget(
					Controller& a_controller);

				void DrawOutfitEntryWidget(
					entryOutfitData_t&                                   a_data,
					const UIOutfitEntryEditorWidgetParamWrapper<_Param>& a_params);

				virtual void EditorDrawMenuBarItems() override;

			private:
				virtual void OnOutfitEntryChange(
					const change_param_type& a_params) = 0;

				virtual void OnOutfitEntryErase(
					const _Param& a_params);

				virtual constexpr Data::ConfigClass GetConfigClass() const = 0;

				virtual constexpr bool IsProfileEditor() const;

				virtual void DrawEquipmentOverrideValues(
					const change_param_type&  a_params,
					Data::OM::configOutfit_t& a_data) override;

				virtual void OnEquipmentOverrideChange(
					const change_param_type& a_params) override;

				virtual void DrawExtraEditorPanelSettings(const void* a_params) override;

				virtual void DrawMainHeaderControlsExtra(
					const change_param_type& a_params);

				void DrawOutfitEntry(
					const change_param_type& a_params);

				bool DrawValues(
					const change_param_type&  a_params,
					Data::OM::configOutfit_t& a_data);

				UIFormPickerWidget m_formPicker;
			};

			template <class _Param>
			UIOutfitEntryEditorWidget<_Param>::UIOutfitEntryEditorWidget(
				Controller& a_controller) :
				UIFormLookupInterface(a_controller),
				m_formPicker(a_controller, FormInfoFlags::kNone, true)
			{
				m_formPicker.SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Outfit));
				m_formPicker.SetFormBrowserEnabled(true);
			}

			template <class _Param>
			void UIOutfitEntryEditorWidget<_Param>::DrawOutfitEntryWidget(
				entryOutfitData_t&                                   a_data,
				const UIOutfitEntryEditorWidgetParamWrapper<_Param>& a_params)
			{
				const change_param_type params{
					a_params,
					GetSex(),
					a_data
				};

				ImGui::PushID("otft_editor_widget");

				DrawEditorPanelSettings(std::addressof(params));

				ImGui::Spacing();

				ImGui::PushID("extra_header");

				DrawMainHeaderControlsExtra(params);

				ImGui::PopID();

				ImGui::Separator();
				ImGui::Spacing();

				if (ImGui::BeginChild("otft_editor_panel", { -1.0f, 0.0f }))
				{
					ImGui::PushItemWidth(ImGui::GetFontSize() * -10.5f);

					const bool dim = a_params.noData;

					if (dim)
					{
						ImGui::PushStyleVar(
							ImGuiStyleVar_Alpha,
							ImGui::GetStyle().Alpha * 0.5f);
					}

					DrawOutfitEntry(params);

					if (dim)
					{
						ImGui::PopStyleVar();
					}

					ImGui::PopItemWidth();
				}

				ImGui::EndChild();

				ImGui::PopID();
			}

			template <class _Param>
			void UIOutfitEntryEditorWidget<_Param>::OnOutfitEntryErase(const _Param& a_params)
			{
			}

			template <class _Param>
			constexpr bool UIOutfitEntryEditorWidget<_Param>::IsProfileEditor() const
			{
				return false;
			}

			template <class _Param>
			void UIOutfitEntryEditorWidget<_Param>::DrawEquipmentOverrideValues(
				const change_param_type&  a_params,
				Data::OM::configOutfit_t& a_data)
			{
				if (DrawValues(a_params, a_data))
				{
					OnOutfitEntryChange(a_params);
				}
			}

			template <class _Param>
			void UIOutfitEntryEditorWidget<_Param>::OnEquipmentOverrideChange(
				const change_param_type& a_params)
			{
				OnOutfitEntryChange(a_params);
			}

			template <class _Param>
			void UIOutfitEntryEditorWidget<_Param>::EditorDrawMenuBarItems()
			{
			}

			template <class _Param>
			void UIOutfitEntryEditorWidget<_Param>::DrawExtraEditorPanelSettings(const void* a_params)
			{
				const auto params = reinterpret_cast<const change_param_type*>(a_params);

				ImGui::SameLine();
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine();

				const bool disabled = params->params.noData;

				std::string dialogName;

				if (!IsProfileEditor())
				{
					UICommon::PushDisabled(disabled);

					dialogName = UIL::LS<CommonStrings, 3>(CommonStrings::Confirm, "d_1");

					if (ImGui::Button(UIL::LS(CommonStrings::Delete, "1")))
					{
						ImGui::OpenPopup(dialogName.c_str());
					}

					UICommon::PopDisabled(disabled);

					if (UICommonModals::ConfirmDialog(
							dialogName.c_str(),
							UICommonModals::cm_func_t(),
							std::optional<float>{},
							"%s",
							UIL::LS(CommonStrings::Delete)) == UICommonModals::ModalStatus::kAccept)
					{
						OnOutfitEntryErase(params->params.actual);
					}

					ImGui::SameLine();
				}

				dialogName = UIL::LS<CommonStrings, 3>(CommonStrings::Confirm, "d_2");

				UICommon::PushDisabled(disabled);

				if (ImGui::Button(UIL::LS(CommonStrings::Clear, "2")))
				{
					ImGui::OpenPopup(dialogName.c_str());
				}

				UICommon::PopDisabled(disabled);

				if (UICommonModals::ConfirmDialog(
						dialogName.c_str(),
						UICommonModals::cm_func_t(),
						std::optional<float>{},
						"%s",
						UIL::LS(CommonStrings::Delete)) == UICommonModals::ModalStatus::kAccept)
				{
					params->data.clear();

					OnOutfitEntryChange(*params);
				}
			}

			template <class _Param>
			void UIOutfitEntryEditorWidget<_Param>::DrawMainHeaderControlsExtra(
				const change_param_type& a_params)
			{
			}

			template <class _Param>
			void UIOutfitEntryEditorWidget<_Param>::DrawOutfitEntry(
				const change_param_type& a_params)
			{
				auto& data = a_params.data.data.get(a_params.sex);

				if (DrawValues(a_params, data))
				{
					OnOutfitEntryChange(a_params);
				}

				this->DrawEquipmentOverrides(data, data.overrides, a_params);
			}

			template <class _Param>
			bool UIOutfitEntryEditorWidget<_Param>::DrawValues(
				const change_param_type&  a_params,
				Data::OM::configOutfit_t& a_data)
			{
				bool result = false;

				ImGui::PushID("values");

#if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
				if (ImGui::Button("T"))
				{
					a_data.flags.toggle(Data::OM::ConfigOutfitFlags::kUseID);
					result = true;
				}

				ImGui::SameLine();

				if (a_data.flags.test(Data::OM::ConfigOutfitFlags::kUseID))
				{
					if (ImGui::BeginCombo(
							"##1",
							a_data.id.c_str(),
							ImGuiComboFlags_HeightLarge))
					{
						auto l = IED::OM::PersistentOutfitFormManager::GetSingleton().GetIDs();

						std::sort(l.begin(), l.end(), stl::fixed_string::less_str{});

						int i = 0;

						ImGui::PushID(i);

						if (ImGui::Selectable(
								"##none"))
						{
							a_data.id.clear();
							result = true;
						}

						ImGui::PopID();

						++i;

						for (auto& e : l)
						{
							ImGui::PushID(i);

							const bool selected = (e == a_data.id);
							if (selected)
							{
								if (ImGui::IsWindowAppearing())
									ImGui::SetScrollHereY();
							}

							if (ImGui::Selectable(
									e.c_str(),
									selected))
							{
								a_data.id = e;
								result    = true;
							}

							ImGui::PopID();

							++i;
						}

						ImGui::EndCombo();
					}
				}
				else
				{
#endif
					result |= m_formPicker.DrawFormPicker(
						"2",
						static_cast<Localization::StringID>(CommonStrings::Form),
						a_data.outfit);
#if defined(IED_ENABLE_OUTFIT_FORM_MANAGER)
				}
#endif

				ImGui::PopID();

				return result;
			}
		}
	}
}