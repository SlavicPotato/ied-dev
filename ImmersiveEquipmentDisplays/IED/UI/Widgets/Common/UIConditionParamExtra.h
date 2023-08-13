#pragma once

#include "IED/UI/UITips.h"

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorWidgetStrings.h"

#include "IED/UI/Widgets/UIConditionParamEditorWidget.h"
#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		template <class Tc, class Tf>
		class UIConditionParamExtra
		{
		protected:
			static void DrawConditionParamExtra_BipedSlot(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Hand(Tc* a_match, UIConditionParamEditorWidget& a_cpeWidget, bool& a_result);
			static void DrawConditionParamExtra_NPC(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Extra(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Cell(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Mounting(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Skeleton(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Race(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Location(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Worldspace(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Idle(Tc* a_match, bool& a_result);
			static void DrawConditionParamExtra_Furniture(Tc* a_match, bool& a_result);
		};

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_BipedSlot(Tc* a_match, bool& a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UINodeOverrideEditorWidgetStrings::MatchSkin, "1"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag2));

			UITipsInterface::DrawTip(UITip::MatchSkin);

			bool disabled = !a_match->flags.test(Tf::kExtraFlag1);

			UICommon::PushDisabled(disabled);

			a_result |= ImGui::CheckboxFlagsT(
				"!##2",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch3));

			UICommon::PopDisabled(disabled);

			ImGui::SameLine();

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UINodeOverrideEditorWidgetStrings::IsBolt, "3"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag1));

			disabled = !a_match->flags.test(Tf::kExtraFlag3);

			UICommon::PushDisabled(disabled);

			a_result |= ImGui::CheckboxFlagsT(
				"!##4",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch4));

			UICommon::PopDisabled(disabled);

			ImGui::SameLine();

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::GeometryVisible, "5"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag3));
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Hand(
			Tc*                           a_match,
			UIConditionParamEditorWidget& a_cpeWidget,
			bool&                         a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				"!##0_1",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch4));

			ImGui::SameLine();

			a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::EquipSlot));
			a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);

			a_result |= a_cpeWidget.GetFormPicker().DrawFormPicker(
				"0_2",
				static_cast<Localization::StringID>(UIFormBrowserStrings::EquipSlot),
				a_match->form2);

			std::optional<std::uint8_t> tmp;
			if (a_match->formType)
			{
				tmp.emplace(a_match->formType);
			}

			UICommon::PushDisabled(!tmp);

			a_result |= ImGui::CheckboxFlagsT(
				"!##1",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch3));

			UICommon::PopDisabled(!tmp);

			ImGui::SameLine();

			if (UIFormTypeSelectorWidget::DrawFormTypeSelector(
					tmp,
					true,
					[](std::uint8_t a_type) {
						return IFormCommon::IsEquippableHandFormType(a_type);
					}))
			{
				a_match->formType = tmp ? *tmp : 0;
				a_result          = true;
			}

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Left, "2"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kMatchLeftHand));
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_NPC(Tc* a_match, bool& a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::MatchNPCOrTemplate, "1"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag1));

			UITipsInterface::DrawTip(UITip::MatchNPCOrTemplate);
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Extra(
			Tc*   a_match,
			bool& a_result)
		{
			switch (a_match->extraCondType)
			{
			case Data::ExtraConditionType::kSunAngle:

				a_result |= ImGui::CheckboxFlagsT(
					UIL::LS(CommonStrings::Absolute, "1"),
					stl::underlying(std::addressof(a_match->flags.value)),
					stl::underlying(Tf::kExtraFlag1));

				ImGui::Spacing();
				ImGui::Text("%s:", UIL::LS(CommonStrings::Info));
				ImGui::SameLine();
				UITipsInterface::DrawTip(UITip::SunAngle);

				break;
			}
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Cell(
			Tc*   a_match,
			bool& a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				"!##1",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch3));

			ImGui::SameLine();

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::Interior, "2"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag1));

			a_result |= ImGui::CheckboxFlagsT(
				"!##3",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch4));

			ImGui::SameLine();

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::PublicArea, "4"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag2));

			a_result |= ImGui::CheckboxFlagsT(
				"!##5",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch6));

			ImGui::SameLine();

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::UsesSkyLighting, "6"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag3));
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Mounting(
			Tc*   a_match,
			bool& a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::IsMountedActorHorse, "1"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag1));
		}

		template <class Tc, class Tf>
		inline void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Skeleton(
			Tc*   a_match,
			bool& a_result)
		{
			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::ID, "1"),
					!a_match->flags.test(Tf::kExtraFlag1)))
			{
				a_result = true;
				a_match->flags.clear(Tf::kExtraFlag1);
			}

			ImGui::SameLine();

			if (ImGui::RadioButton(
					UIL::LS(CommonStrings::Signature, "2"),
					a_match->flags.test(Tf::kExtraFlag1)))
			{
				a_result = true;
				a_match->flags.set(Tf::kExtraFlag1);
			}

			ImGui::Separator();
			ImGui::Spacing();

			if (a_match->flags.test(Tf::kExtraFlag1))
			{
				a_result |= ImGui::InputScalar(
					UIL::LS(CommonStrings::Signature, "3"),
					ImGuiDataType_U64,
					std::addressof(a_match->skeletonSignature),
					nullptr,
					nullptr,
					"%llu",
					ImGuiInputTextFlags_EnterReturnsTrue |
						ImGuiInputTextFlags_CharsDecimal);
			}
			else
			{
				a_result |= ImGui::InputScalar(
					UIL::LS(CommonStrings::ID, "3"),
					ImGuiDataType_S32,
					std::addressof(a_match->skeletonID),
					nullptr,
					nullptr,
					"%d",
					ImGuiInputTextFlags_EnterReturnsTrue |
						ImGuiInputTextFlags_CharsDecimal);
			}
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Race(
			Tc*   a_match,
			bool& a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				"!##1",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch3));

			ImGui::SameLine();

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::IsPlayable, "2"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag1));

			a_result |= ImGui::CheckboxFlagsT(
				"!##3",
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kNegateMatch4));

			ImGui::SameLine();

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::ChildRace, "4"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag2));
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Location(
			Tc*   a_match,
			bool& a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::MatchParent, "1"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Data::NodeOverrideConditionFlags::kExtraFlag1));

			UITipsInterface::DrawTip(UITip::MatchChildLoc);

			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::MatchEither, "2"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Data::NodeOverrideConditionFlags::kMatchCategoryOperOR));

			UITipsInterface::DrawTip(UITip::MatchEitherFormKW);
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Worldspace(
			Tc*   a_match,
			bool& a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(UIWidgetCommonStrings::MatchParent, "1"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag1));

			UITipsInterface::DrawTip(UITip::MatchWorldspaceParent);
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Idle(
			Tc*   a_match,
			bool& a_result)
		{
			ImGui::Spacing();
			ImGui::Text("%s:", UIL::LS(CommonStrings::Info));
			ImGui::SameLine();
			UITipsInterface::DrawTip(UITip::IdleCondition);
		}

		template <class Tc, class Tf>
		void UIConditionParamExtra<Tc, Tf>::DrawConditionParamExtra_Furniture(
			Tc* a_match, bool& a_result)
		{
			a_result |= ImGui::CheckboxFlagsT(
				UIL::LS(CommonStrings::LayingDown, "1"),
				stl::underlying(std::addressof(a_match->flags.value)),
				stl::underlying(Tf::kExtraFlag1));
		}
	}
}