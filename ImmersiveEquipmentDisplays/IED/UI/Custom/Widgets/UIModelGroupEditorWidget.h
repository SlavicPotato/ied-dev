#pragma once

#include "IED/UI/UILocalizationInterface.h"
#include "IED/UI/UINotificationInterface.h"
#include "IED/UI/UITips.h"
#include "IED/UI/Widgets/Form/UIFormPickerWidget.h"
#include "IED/UI/Widgets/UIBaseConfigWidgetStrings.h"
#include "IED/UI/Widgets/UIDescriptionPopup.h"
#include "IED/UI/Widgets/UITransformSliderWidget.h"

#include "UIModelGroupEditorWidgetStrings.h"

#include "IED/ConfigOverrideModelGroup.h"

#include "UISingleCustomConfigUpdateParams.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		enum class ModelGroupEditorOnChangeEventType
		{
			Form,
			Flags,
			Transform
		};

		template <class T>
		class UIModelGroupEditorWidget :
			virtual public UIDescriptionPopupWidget,
			virtual public UITipsInterface,
			virtual public UITransformSliderWidget,
			virtual public UINotificationInterface,
			virtual public UILocalizationInterface
		{
		public:
			UIModelGroupEditorWidget(
				UIFormPickerWidget& a_formPicker,
				Controller&         a_controller);

			void DrawModelGroupEditorWidget(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params,
				Data::configModelGroup_t&             a_data);

			void DrawModelGroupEditorWidgetTree(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params,
				Data::configModelGroup_t&             a_data);

		protected:
			void CreatePrimaryModelGroup(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params,
				Data::configModelGroup_t&             a_data);

		private:
			void DrawEntry(
				T                                                a_handle,
				const SingleCustomConfigUpdateParams&            a_params,
				Data::configModelGroup_t&                        a_data,
				Data::configModelGroup_t::data_type::value_type& a_value);

			void DrawFlags(
				T                                                a_handle,
				const SingleCustomConfigUpdateParams&            a_params,
				Data::configModelGroup_t&                        a_data,
				Data::configModelGroup_t::data_type::value_type& a_value);

			void DrawHeaderContextMenu(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params,
				Data::configModelGroup_t&             a_data);

			void DrawEntryHeaderContextMenu(
				T                                              a_handle,
				const SingleCustomConfigUpdateParams&          a_params,
				Data::configModelGroup_t&                      a_data,
				Data::configModelGroup_t::data_type::iterator& a_it);

			virtual void OnModelGroupEditorChange(
				T                                     a_handle,
				const SingleCustomConfigUpdateParams& a_params,
				ModelGroupEditorOnChangeEventType     a_type) = 0;

			UIFormPickerWidget& m_formPicker;
		};

		template <class T>
		UIModelGroupEditorWidget<T>::UIModelGroupEditorWidget(
			UIFormPickerWidget& a_formPicker,
			Controller&         a_controller) :
			UILocalizationInterface(a_controller),
			UINotificationInterface(a_controller),
			m_formPicker(a_formPicker)
		{
		}

		template <class T>
		void UIModelGroupEditorWidget<T>::DrawModelGroupEditorWidget(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params,
			Data::configModelGroup_t&             a_data)
		{
			auto& data = a_params.entry(a_params.sex);

			constexpr auto TREE_FLAGS = ImGuiTreeNodeFlags_DefaultOpen |
			                            ImGuiTreeNodeFlags_SpanAvailWidth;

			ImGui::PushID("mged_w");

			for (auto it = a_data.entries.begin(); it != a_data.entries.end(); ++it)
			{
				ImGui::PushID(it->first.c_str());

				DrawEntryHeaderContextMenu(a_handle, a_params, a_data, it);

				if (it != a_data.entries.end())
				{
					bool result;

					if (it->first.empty())
					{
						ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorLightBlue);

						result = ImGui::TreeNodeEx(
							"e_tree",
							TREE_FLAGS,
							"%s [%.8X]",
							LS(CommonStrings::Primary),
							it->second.form.get_id().get());

						ImGui::PopStyleColor();
					}
					else
					{
						result = ImGui::TreeNodeEx(
							"e_tree",
							TREE_FLAGS,
							"%s",
							it->first.c_str());
					}

					if (result)
					{
						ImGui::Spacing();
						ImGui::Indent();

						DrawEntry(a_handle, a_params, a_data, *it);

						ImGui::Unindent();
						ImGui::Spacing();

						ImGui::TreePop();
					}
				}

				ImGui::PopID();
			}

			ImGui::PopID();
		}

		template <class T>
		void UIModelGroupEditorWidget<T>::DrawModelGroupEditorWidgetTree(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params,
			Data::configModelGroup_t&             a_data)
		{
			ImGui::PushID("mg_area");

			DrawHeaderContextMenu(a_handle, a_params, a_data);

			if (ImGui::TreeNodeEx(
					"tree",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(UICustomEditorString::ModelGroup)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				DrawModelGroupEditorWidget(
					a_handle,
					a_params,
					a_data);

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::TreePop();
			}

			ImGui::PopID();
		}

		template <class T>
		void UIModelGroupEditorWidget<T>::CreatePrimaryModelGroup(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params,
			Data::configModelGroup_t&             a_data)
		{
			auto& data = a_params.entry(a_params.sex);

			auto r = a_data.entries.try_emplace({}, Data::ConfigModelGroupEntryFlags::kNone, data.form);

			if (r.first->second.form.get_id() != data.form.get_id())
			{
				r.first->second.form = data.form.get_id();
			}
		}

		template <class T>
		void UIModelGroupEditorWidget<T>::DrawEntry(
			T                                                a_handle,
			const SingleCustomConfigUpdateParams&            a_params,
			Data::configModelGroup_t&                        a_data,
			Data::configModelGroup_t::data_type::value_type& a_value)
		{
			auto& entry = a_value.second;
			auto& data  = a_params.entry(a_params.sex);

			if (m_formPicker.DrawFormPicker(
					"fp",
					LS(CommonStrings::Form),
					entry.form))
			{
				if (a_value.first.empty())
				{
					data.form = entry.form.get_id();
				}

				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Form);
			}

			ImGui::Spacing();

			if (ImGui::TreeNodeEx(
					"flg",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Flags)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				DrawFlags(a_handle, a_params, a_data, a_value);

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx(
					"xfrm",
					ImGuiTreeNodeFlags_DefaultOpen |
						ImGuiTreeNodeFlags_SpanAvailWidth,
					"%s",
					LS(CommonStrings::Transform)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				DrawTransformSliders(
					entry.transform,
					[&](auto a_v) {
						OnModelGroupEditorChange(
							a_handle,
							a_params,
							ModelGroupEditorOnChangeEventType::Transform);
					});

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::TreePop();
			}
		}
		template <class T>
		void UIModelGroupEditorWidget<T>::DrawFlags(
			T                                                a_handle,
			const SingleCustomConfigUpdateParams&            a_params,
			Data::configModelGroup_t&                        a_data,
			Data::configModelGroup_t::data_type::value_type& a_value)
		{
			auto& entry = a_value.second;

			ImGui::PushID("flags");

			ImGui::Columns(2, nullptr, false);

			if (ImGui::CheckboxFlagsT(
					LS(UIBaseConfigString::DropOnDeath, "1"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kDropOnDeath)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			DrawTip(UITip::DropOnDeath);

			if (ImGui::CheckboxFlagsT(
					LS(UIBaseConfigString::KeepTorchFlame, "2"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kKeepTorchFlame)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			DrawTip(UITip::KeepTorchFlame);

			if (ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::LeftWeapon, "3"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kLeftWeapon)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			DrawTip(UITip::LeftWeapon);

			ImGui::NextColumn();

			if (ImGui::CheckboxFlagsT(
					LS(UIBaseConfigString::Use1pWeaponModels, "4"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kLoad1pWeaponModel)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			DrawTip(UITip::Load1pWeaponModel);

			if (ImGui::CheckboxFlagsT(
					LS(UIBaseConfigString::UseWorldModel, "5"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kUseWorldModel)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			DrawTip(UITip::UseWorldModel);

			if (ImGui::CheckboxFlagsT(
					LS(UIBaseConfigString::RemoveScabbard, "6"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kRemoveScabbard)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			DrawTip(UITip::RemoveScabbard);

			if (ImGui::CheckboxFlagsT(
					LS(UIWidgetCommonStrings::DisableHavok, "7"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kDisableHavok)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			DrawTipWarn(UITip::DisableHavok);

			ImGui::Columns();

			ImGui::PopID();
		}

		template <class T>
		void UIModelGroupEditorWidget<T>::DrawHeaderContextMenu(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params,
			Data::configModelGroup_t&             a_data)
		{
			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			if (DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_BM(CommonStrings::New, "1"))
				{
					if (DrawDescriptionPopup())
					{
						auto r = a_data.entries.try_emplace(GetDescriptionPopupBuffer());
						if (!r.second)
						{
							QueueNotification(
								LS(CommonStrings::Error),
								"%s",
								LS(UIModelGroupEditorWidgetStrings::EntryExistsError));
						}

						ClearDescriptionPopupBuffer();
					}

					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}

		template <class T>
		void UIModelGroupEditorWidget<T>::DrawEntryHeaderContextMenu(
			T                                              a_handle,
			const SingleCustomConfigUpdateParams&          a_params,
			Data::configModelGroup_t&                      a_data,
			Data::configModelGroup_t::data_type::iterator& a_it)
		{
			ImGui::PushID("context_area");

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 4.f, 1.0f });

			DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (LCG_MI(CommonStrings::Reset, "1"))
				{
					a_it->second = {};

					OnModelGroupEditorChange(
						a_handle,
						a_params,
						ModelGroupEditorOnChangeEventType::Form);
				}

				if (!a_it->first.empty())
				{
					if (LCG_MI(CommonStrings::Delete, "2"))
					{
						a_it = a_data.entries.erase(a_it);

						OnModelGroupEditorChange(
							a_handle,
							a_params,
							ModelGroupEditorOnChangeEventType::Form);
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopID();
		}
	}
}