#pragma once

#include "IED/UI/UILocalizationInterface.h"
#include "IED/UI/UIPopupInterface.h"
#include "IED/UI/UITips.h"
#include "IED/UI/Widgets/Form/UIFormPickerWidget.h"
#include "IED/UI/Widgets/UIBaseConfigWidgetStrings.h"
#include "IED/UI/Widgets/UIDescriptionPopup.h"
#include "IED/UI/Widgets/UIPopupToggleButtonWidget.h"
#include "IED/UI/Widgets/UITransformSliderWidget.h"

#include "UIModelGroupEditorWidgetStrings.h"

#include "IED/ConfigModelGroup.h"

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
			virtual public UITransformSliderWidget,
			virtual public UIPopupInterface
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
			m_formPicker(a_formPicker)
		{
		}

		template <class T>
		void UIModelGroupEditorWidget<T>::DrawModelGroupEditorWidget(
			T                                     a_handle,
			const SingleCustomConfigUpdateParams& a_params,
			Data::configModelGroup_t&             a_data)
		{
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
							UIL::LS(CommonStrings::Primary),
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
					UIL::LS(UICustomEditorString::ModelGroup)))
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
					static_cast<Localization::StringID>(CommonStrings::Form),
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
					UIL::LS(CommonStrings::Flags)))
			{
				ImGui::Spacing();
				ImGui::Indent();

				DrawFlags(a_handle, a_params, a_data, a_value);

				ImGui::Unindent();
				ImGui::Spacing();

				ImGui::TreePop();
			}

			bool disabled = entry.flags.test(Data::ConfigModelGroupEntryFlags::kDisabled);

			UICommon::PushDisabled(disabled);

			DrawTransformTree(
				entry.transform,
				false,
				[&](auto a_v) {
					OnModelGroupEditorChange(
						a_handle,
						a_params,
						ModelGroupEditorOnChangeEventType::Transform);
				},
				[] {});

			UICommon::PopDisabled(disabled);
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
					UIL::LS(CommonStrings::Disable, "0"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kDisabled)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}

			bool disabled = entry.flags.test(Data::ConfigModelGroupEntryFlags::kDisabled);

			UICommon::PushDisabled(disabled);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIBaseConfigString::DropOnDeath, "1"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kDropOnDeath)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::DropOnDeath);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIBaseConfigString::KeepTorchFlame, "2"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kKeepTorchFlame)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::KeepTorchFlame);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::LeftWeapon, "3"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kLeftWeapon)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::LeftWeapon);

			bool paChanged = ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::PlayAnimation, "4"),
				stl::underlying(std::addressof(entry.flags.value)),
				stl::underlying(Data::ConfigModelGroupEntryFlags::kPlaySequence));

			if (paChanged)
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::PlayAnimation);

			if (entry.flags.test(Data::ConfigModelGroupEntryFlags::kPlaySequence))
			{
				if (paChanged &&
				    entry.niControllerSequence.empty())
				{
					ImGui::OpenPopup("4ctx");
					ClearDescriptionPopupBuffer();
				}

				ImGui::SameLine();
				if (UIPopupToggleButtonWidget::DrawPopupToggleButton("4b", "4ctx"))
				{
					SetDescriptionPopupBuffer(entry.niControllerSequence);
				}

				if (ImGui::BeginPopup("4ctx"))
				{
					if (DrawDescriptionPopup(UIL::LS(CommonStrings::Sequence, "1")))
					{
						entry.niControllerSequence = GetDescriptionPopupBuffer();

						OnModelGroupEditorChange(
							a_handle,
							a_params,
							ModelGroupEditorOnChangeEventType::Flags);
					}

					ImGui::EndPopup();
				}
			}

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIBaseConfigString::Use1pWeaponModels, "A"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kLoad1pWeaponModel)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::Load1pWeaponModel);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::RemoveEditorMarker, "_1"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kRemoveEditorMarker)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			
			ImGui::NextColumn();

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIBaseConfigString::UseWorldModel, "B"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kUseWorldModel)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::UseWorldModel);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIBaseConfigString::RemoveScabbard, "C"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kRemoveScabbard)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::RemoveScabbard);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::DisableWeaponAnims, "D"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kDisableWeaponAnims)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::DisableWeaponAnims);

			/*if (ImGui::CheckboxFlagsT(
					UILI::LS(UIWidgetCommonStrings::DisableAnimEventForwarding, "E"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kDisableAnimEventForwarding)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::DisableAnimEventForwarding);*/

			paChanged = ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::AnimationEvent, "F"),
				stl::underlying(std::addressof(entry.flags.value)),
				stl::underlying(Data::ConfigModelGroupEntryFlags::kAnimationEvent));

			if (paChanged)
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Transform);
			}
			UITipsInterface::DrawTip(UITip::AnimationEvent);

			if (entry.flags.test(Data::ConfigModelGroupEntryFlags::kAnimationEvent))
			{
				if (paChanged &&
				    entry.animationEvent.empty())
				{
					ImGui::OpenPopup("Fctx");
					ClearDescriptionPopupBuffer();
				}

				ImGui::SameLine();
				if (UIPopupToggleButtonWidget::DrawPopupToggleButton("Fb", "Fctx"))
				{
					SetDescriptionPopupBuffer(entry.animationEvent);
				}

				if (ImGui::BeginPopup("Fctx"))
				{
					if (DrawDescriptionPopup(UIL::LS(CommonStrings::Event, "1")))
					{
						entry.animationEvent = GetDescriptionPopupBuffer();

						OnModelGroupEditorChange(
							a_handle,
							a_params,
							ModelGroupEditorOnChangeEventType::Transform);
					}

					ImGui::EndPopup();
				}
			}

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::ForceTryLoadAnim, "F1"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kForceTryLoadAnim)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::ForceTryLoadAnim);

			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::DisableHavok, "G"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kDisableHavok)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::DisableHavok);
			
			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::RemoveProjectileTracers, "H"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kRemoveProjectileTracers)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::RemoveProjectileTracers);
			
			if (ImGui::CheckboxFlagsT(
					UIL::LS(UIWidgetCommonStrings::AttachLight, "I"),
					stl::underlying(std::addressof(entry.flags.value)),
					stl::underlying(Data::ConfigModelGroupEntryFlags::kAttachLight)))
			{
				OnModelGroupEditorChange(
					a_handle,
					a_params,
					ModelGroupEditorOnChangeEventType::Flags);
			}
			UITipsInterface::DrawTip(UITip::AttachLight);

			UICommon::PopDisabled(disabled);

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

			if (UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu"))
			{
				ClearDescriptionPopupBuffer();
			}

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_BM(CommonStrings::New, "1"))
				{
					if (DrawDescriptionPopup())
					{
						auto r = a_data.entries.try_emplace(GetDescriptionPopupBuffer());
						if (!r.second)
						{
							QueueNotification(
								UIL::LS(CommonStrings::Error),
								"%s",
								UIL::LS(UIModelGroupEditorWidgetStrings::EntryExistsError));
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

			UIPopupToggleButtonWidget::DrawPopupToggleButton("open", "context_menu");

			ImGui::PopStyleVar();

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (ImGui::BeginPopup("context_menu"))
			{
				if (UIL::LCG_MI(CommonStrings::Reset, "1"))
				{
					a_it->second = {};

					OnModelGroupEditorChange(
						a_handle,
						a_params,
						ModelGroupEditorOnChangeEventType::Form);
				}

				if (!a_it->first.empty())
				{
					if (UIL::LCG_MI(CommonStrings::Delete, "2"))
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