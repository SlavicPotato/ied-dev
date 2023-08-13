#pragma once

#include "IED/UI/UITips.h"

#include "IED/UI/NodeOverride/Widgets/UINodeOverrideEditorWidgetStrings.h"
#include "IED/UI/UIFormBrowserCommonFilters.h"

#include "IED/UI/Widgets/UIConditionParamEditorWidget.h"
#include "IED/UI/Widgets/UIWidgetCommonStrings.h"

namespace IED
{
	namespace UI
	{
		template <class Tc, class Tt, class Tf>
		class UIConditionItemExtra
		{
		protected:
			static void DrawConditionItemExtra_All(
				ConditionParamItem           a_item,
				ConditionParamItemExtraArgs& a_args,
				bool&                        a_result);

			static void DoUpdateMatchParamAllowedTypes(
				Tc&                           a_config,
				UIConditionParamEditorWidget& a_cpeWidget);

			static void UpdateMatchParamAllowedTypesImpl(
				Tt                            a_type,
				UIConditionParamEditorWidget& a_cpeWidget);

			static void OnConditionItemChangeImpl(
				ConditionParamItem                    a_item,
				const ConditionParamItemOnChangeArgs& a_args);
		};

		template <class Tc, class Tt, class Tf>
		void UIConditionItemExtra<Tc, Tt, Tf>::DrawConditionItemExtra_All(
			ConditionParamItem           a_item,
			ConditionParamItemExtraArgs& a_args,
			bool&                        a_result)
		{
			auto match = static_cast<Tc*>(a_args.p3);

			switch (match->flags.bf().type)
			{
			case Tt::BipedSlot:
			case Tt::Type:
			case Tt::Furniture:
			case Tt::Location:
			case Tt::Package:

				if (a_item == ConditionParamItem::Form)
				{
					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch1));

					if (match->flags.bf().type == Tt::BipedSlot &&
					    match->flags.test(Tf::kExtraFlag2))
					{
						a_args.disable = true;
					}

					ImGui::SameLine();
				}
				else if (
					a_item == ConditionParamItem::Keyword ||
					a_item == ConditionParamItem::PackageType)
				{
					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch2));

					ImGui::SameLine();
				}

				break;

			case Tt::Form:

				if (a_item == ConditionParamItem::Keyword)
				{
					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch1));

					ImGui::SameLine();
				}

				break;
			case Tt::Effect:

				if (a_item == ConditionParamItem::FormAny)
				{
					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_fa_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch3));

					ImGui::SameLine();

					a_args.formFilter = UIFormBrowserFilter::EffectSource;
				}

				[[fallthrough]];

			case Tt::Actor:
			case Tt::NPC:
			case Tt::Race:
			case Tt::Idle:
			case Tt::Cell:
			case Tt::Hand:

				if (a_item == ConditionParamItem::Form)
				{
					a_result |= ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch2));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::Keyword)
				{
					a_result |= ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch1));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::LightingTemplateInheritanceFlags)
				{
					a_result |= ImGui::CheckboxFlagsT(
						"!##ctl_neg_3",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch5));

					/*ImGui::SameLine();

					result |= ImGui::CheckboxFlagsT(
						"##ctl_sw_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kExtraFlag3));

					if (!match->flags.test(Tf::kExtraFlag3))
					{
						a_args.disable = true;
					}*/

					ImGui::SameLine();
				}

				break;

			case Tt::Extra:

				if (a_item == ConditionParamItem::Form)
				{
					switch (match->extraCondType)
					{
					case Data::ExtraConditionType::kShoutEquipped:
					case Data::ExtraConditionType::kCombatStyle:
					case Data::ExtraConditionType::kClass:
					case Data::ExtraConditionType::kLightingTemplate:

						a_result = ImGui::CheckboxFlagsT(
							"!##ctl_neg_1",
							stl::underlying(std::addressof(match->flags.value)),
							stl::underlying(Tf::kNegateMatch1));

						ImGui::SameLine();

						a_args.hide = false;

						break;
					default:
						a_args.hide = true;
						break;
					}
				}

				break;

			case Tt::Weather:

				if (a_item == ConditionParamItem::Form)
				{
					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch1));

					ImGui::SameLine();
				}
				else if (a_item == ConditionParamItem::WeatherClass)
				{
					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch2));

					ImGui::SameLine();
				}

				break;
			case Tt::Mounting:
			case Tt::Mounted:

				switch (a_item)
				{
				case ConditionParamItem::Form:

					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_1",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch1));

					ImGui::SameLine();

					break;

				case ConditionParamItem::Race:

					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_2",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch2));

					ImGui::SameLine();

					break;

				case ConditionParamItem::Keyword:

					a_result = ImGui::CheckboxFlagsT(
						"!##ctl_neg_3",
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kNegateMatch3));

					ImGui::SameLine();

					break;
				}

				break;

			case Tt::Faction:
			case Tt::Perk:

				switch (a_item)
				{
				case ConditionParamItem::CompOper:

					a_result = ImGui::CheckboxFlagsT(
						UIL::LS(CommonStrings::Rank, "ctl_tog_1"),
						stl::underlying(std::addressof(match->flags.value)),
						stl::underlying(Tf::kExtraFlag1));

					if (match->flags.test(Tf::kExtraFlag1))
					{
						ImGui::SameLine();
					}

					[[fallthrough]];
				case ConditionParamItem::Int32:

					a_args.hide = !match->flags.test(Tf::kExtraFlag1);

					break;
				}

				break;

			case Tt::Variable:

				switch (a_item)
				{
				case ConditionParamItem::Int32:

					if (match->condVarType != ConditionalVariableType::kInt32)
					{
						a_args.hide = true;
					}

					break;
				case ConditionParamItem::Float:

					if (match->condVarType != ConditionalVariableType::kFloat)
					{
						a_args.hide = true;
					}

					break;

				case ConditionParamItem::Form:

					switch (match->vcSource)
					{
					case Data::VariableConditionSource::kActor:
					case Data::VariableConditionSource::kNPC:
					case Data::VariableConditionSource::kRace:

						a_result |= ImGui::CheckboxFlagsT(
							"!##ctl_neg_1",
							stl::underlying(std::addressof(match->flags.value)),
							stl::underlying(Tf::kNegateMatch1));

						ImGui::SameLine();

						break;
					default:
						a_args.hide = true;
						break;
					}

					break;

				case ConditionParamItem::VarCondSource:

					if (match->vcSource == Data::VariableConditionSource::kSelf)
					{
						a_result |= ImGui::CheckboxFlagsT(
							"!##ctl_neg_2",
							stl::underlying(std::addressof(match->flags.value)),
							stl::underlying(Tf::kNegateMatch2));

						ImGui::SameLine();
					}

					break;

				case ConditionParamItem::FormAny:

					if (match->condVarType != ConditionalVariableType::kForm)
					{
						a_args.hide = true;
					}

					break;

					/*case ConditionParamItem::CompOper:

					if (match->condVarType == ConditionalVariableType::kForm)
					{
						a_args.hide = true;
					}

					break;*/
				}

				break;
			}
		}

		template <class Tc, class Tt, class Tf>
		inline void UIConditionItemExtra<Tc, Tt, Tf>::DoUpdateMatchParamAllowedTypes(
			Tc&                           a_config,
			UIConditionParamEditorWidget& a_cpeWidget)
		{
			const auto type = a_config.flags.bf().type;

			if (type == Tt::Variable)
			{
				switch (a_config.vcSource)
				{
				case Data::VariableConditionSource::kActor:
					a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
					a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(false);
					break;
				case Data::VariableConditionSource::kNPC:
					a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::NPC));
					a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
					break;
				case Data::VariableConditionSource::kRace:
					a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Race));
					a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
					break;
				default:
					a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Common));
					a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
					break;
				}
			}
			else
			{
				UpdateMatchParamAllowedTypesImpl(type, a_cpeWidget);
			}
		}

		template <class Tc, class Tt, class Tf>
		void UIConditionItemExtra<Tc, Tt, Tf>::UpdateMatchParamAllowedTypesImpl(
			Tt                            a_type,
			UIConditionParamEditorWidget& a_cpeWidget)
		{
			switch (a_type)
			{
			case Tt::Keyword:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Keyword));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Race:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Race));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Furniture:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Furniture));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Quest:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Quest));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Actor:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Actor));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(false);
				break;
			case Tt::NPC:
			case Tt::Mounting:
			case Tt::Mounted:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::NPC));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Location:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Location));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Worldspace:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Worldspace));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Package:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Package));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Weather:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Weather));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Global:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Global));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Idle:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Idle));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(false);
				break;
			case Tt::Faction:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Faction));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Effect:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Effect));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Perk:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Perk));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			case Tt::Cell:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Cell));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(false);
				break;
			case Tt::Hand:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::HandEquippable));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			default:
				a_cpeWidget.GetFormPicker().SetAllowedTypes(UIFormBrowserCommonFilters::Get(UIFormBrowserFilter::Common));
				a_cpeWidget.GetFormPicker().SetFormBrowserEnabled(true);
				break;
			}
		}

		template <class Tc, class Tt, class Tf>
		void UIConditionItemExtra<Tc, Tt, Tf>::OnConditionItemChangeImpl(
			ConditionParamItem                    a_item,
			const ConditionParamItemOnChangeArgs& a_args)
		{
			auto match = static_cast<Tc*>(a_args.p3);

			switch (match->flags.bf().type)
			{
			case Tt::Variable:

				if (a_item == ConditionParamItem::VarCondSource)
				{
					match->form = {};
				}

				break;
			}
		}

	}
}