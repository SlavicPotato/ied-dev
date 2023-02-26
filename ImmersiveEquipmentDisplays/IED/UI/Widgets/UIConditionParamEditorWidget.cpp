#include "pch.h"

#include "UIConditionParamEditorWidget.h"

#include "UIActorValueSelectorWidget.h"
#include "UIBipedObjectSelectorWidget.h"
#include "UICMNodeSelector.h"
#include "UIComparisonOperatorSelector.h"
#include "UIConditionExtraSelectorWidget.h"
#include "UIDayOfWeekSelectorWidget.h"
#include "UIKeyBindIDSelectorWidget.h"
#include "UILifeStateSelectorWidget.h"
#include "UIObjectTypeSelectorWidget.h"
#include "UIPackageTypeSelectorWidget.h"
#include "UIVariableConditionSourceSelectorWidget.h"
#include "UIVariableTypeSelectorWidget.h"

#include "IED/Controller/Controller.h"
#include "IED/Data.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		UIConditionParamEditorWidget::UIConditionParamEditorWidget(Controller& a_controller) :
			UIFormLookupInterface(a_controller),
			m_formPickerForm(a_controller, FormInfoFlags::kNone, true),
			m_formPickerKeyword(a_controller, FormInfoFlags::kNone, true),
			m_formPickerRace(a_controller, FormInfoFlags::kNone, true),
			m_formPickerAny(a_controller, FormInfoFlags::kNone, false)
		{
			m_formPickerKeyword.SetAllowedTypes(
				{ BGSKeyword::kTypeID });
			m_formPickerRace.SetAllowedTypes(
				{ TESRace::kTypeID });
		}

		void UIConditionParamEditorWidget::OpenConditionParamEditorPopup()
		{
			ImGui::OpenPopup(POPUP_ID);
		}

		bool UIConditionParamEditorWidget::DrawConditionParamEditorPopup()
		{
			if (!ImGui::IsPopupOpen(POPUP_ID))
			{
				return false;
			}

			bool result;

			const float w = ImGui::GetFontSize() * 34.0f;

			ImGui::SetNextWindowSizeConstraints(
				{ w, 0.0f },
				{ w, 800.0f });

			if (ImGui::BeginPopup(POPUP_ID))
			{
				result = DrawConditionParamEditorPanel();

				ImGui::EndPopup();
			}
			else
			{
				result = false;
			}

			return result;
		}

		bool UIConditionParamEditorWidget::DrawConditionParamEditorPanel()
		{
			bool result = false;

			ImGui::PushID("cep");

			ImGui::PushItemWidth(ImGui::GetFontSize() * -6.0f);

			GetFormPicker().SetAllowClear(!m_tempFlags.test(UIConditionParamEditorTempFlags::kNoClearForm));
			GetKeywordPicker().SetAllowClear(!m_tempFlags.test(UIConditionParamEditorTempFlags::kNoClearKeyword));

			if (const auto& e = get(ConditionParamItem::CondExtra); e.p1)
			{
				auto c = e.As1<Data::ExtraConditionType>();

				if (UIConditionExtraSelectorWidget::DrawExtraConditionSelector(
						e.As1<Data::ExtraConditionType>()))
				{
					if (c != e.As1<Data::ExtraConditionType>())
					{
						if (const auto& f = get(ConditionParamItem::Form); f.p1)
						{
							f.As1<Game::FormID>() = {};
						}

						if (const auto& f = get(ConditionParamItem::TimeOfDay); f.p1)
						{
							f.As1<Data::TimeOfDay>() = Data::TimeOfDay::kNone;
						}

						if (const auto& f = get(ConditionParamItem::Percent); f.p1)
						{
							f.As1<float>() = 0.0f;
						}

						if (const auto& f = get(ConditionParamItem::NodeMon); f.p1)
						{
							f.As1<std::uint32_t>() = 0;
						}

						if (const auto& f = get(ConditionParamItem::DayOfWeek); f.p1)
						{
							f.As1<RE::Calendar::Day>() = RE::Calendar::Day::kSundas;
						}

						if (const auto& f = get(ConditionParamItem::LifeState); f.p1)
						{
							f.As1<ActorState::ACTOR_LIFE_STATE>() = ActorState::ACTOR_LIFE_STATE::kAlive;
						}

						if (const auto& f = get(ConditionParamItem::ActorValue); f.p1)
						{
							f.As1<RE::ActorValue>() = RE::ActorValue::kOneHanded;
						}

						if (const auto& f = get(ConditionParamItem::CompOper); f.p1)
						{
							f.As1<Data::ComparisonOperator>() = Data::ComparisonOperator::kEqual;
						}

						if (const auto& f = get(ConditionParamItem::KeyBindID); f.p1)
						{
							f.As1<stl::fixed_string>().clear();
						}

						if (const auto& f = get(ConditionParamItem::KeyBindID); f.p1)
						{
							f.As1<stl::fixed_string>().clear();
						}

						if (const auto& f = get(ConditionParamItem::LightingTemplateInheritanceFlags); f.p1)
						{
							f.As1<RE::INTERIOR_DATA::Inherit>() = RE::INTERIOR_DATA::Inherit::kNone;
						}

						if (c == Data::ExtraConditionType::kInteriorAmbientLightLevel)
						{
							if (const auto& f = get(ConditionParamItem::Float); f.p1)
							{
								f.As1<float>() = 0.0f;
							}
						}
					}

					result = true;
				}
			}

			if (const auto& e = get(ConditionParamItem::CMENode); e.p1 && e.p2)
			{
				result |= UICMNodeSelectorWidget::DrawCMNodeSelector(
					UIL::LS(CommonStrings::Node, "ns"),
					e.As1<stl::fixed_string>(),
					NodeOverrideData::GetCMENodeData(),
					static_cast<const stl::fixed_string*>(e.p2));

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::BipedSlot); e.p1)
			{
				result |= UIBipedObjectSelectorWidget::DrawBipedObjectSelector(
					UIL::LS(CommonStrings::Biped, "bp"),
					e.As1<BIPED_OBJECT>(),
					m_tempFlags.test(UIConditionParamEditorTempFlags::kAllowBipedNone));

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::EquipmentSlot); e.p1)
			{
				result |= UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
					UIL::LS(CommonStrings::Slot, "ss"),
					e.As1<Data::ObjectSlot>());

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::EquipmentSlotExtra); e.p1)
			{
				result |= UIObjectSlotSelectorWidget::DrawObjectSlotSelector(
					UIL::LS(CommonStrings::Slot, "sse"),
					e.As1<Data::ObjectSlotExtra>());

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::TimeOfDay); e.p1)
			{
				result |= DrawTimeOfDaySelector(e.As1<Data::TimeOfDay>());
			}

			if (const auto& e = get(ConditionParamItem::NodeMon); e.p1)
			{
				result |= DrawNodeMonitorSelector(e.As1<std::uint32_t>());
			}

			if (const auto& e = get(ConditionParamItem::VarCondSource); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::VarCondSource);

				if (!args.hide)
				{
					if (UIVariableConditionSourceSelectorWidget::DrawVariableConditionSourceSelectorWidget(
							e.As1<Data::VariableConditionSource>()))
					{
						result = true;
						OnChange(e, ConditionParamItem::VarCondSource);
					}
				}
			}

			if (const auto& e = get(ConditionParamItem::Form); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::Form);

				if (!args.hide)
				{
					UICommon::PushDisabled(args.disable);

					auto label =
						e.p2 ?
							static_cast<Localization::StringID>(reinterpret_cast<std::uintptr_t>(e.p2)) :
							static_cast<Localization::StringID>(CommonStrings::Form);

					result |= m_formPickerForm.DrawFormPicker(
						"fp_1",
						label,
						e.As1<Game::FormID>());

					UICommon::PopDisabled(args.disable);
				}
			}

			if (const auto& e = get(ConditionParamItem::Keyword); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::Keyword);

				if (!args.hide)
				{
					UICommon::PushDisabled(args.disable);

					result |= m_formPickerKeyword.DrawFormPicker(
						"fp_2",
						static_cast<Localization::StringID>(CommonStrings::Keyword),
						e.As1<Game::FormID>());

					UICommon::PopDisabled(args.disable);
				}
			}

			if (const auto& e = get(ConditionParamItem::Race); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::Race);

				if (!args.hide)
				{
					UICommon::PushDisabled(args.disable);

					result |= m_formPickerRace.DrawFormPicker(
						"fp_3",
						static_cast<Localization::StringID>(CommonStrings::Race),
						e.As1<Game::FormID>());

					UICommon::PopDisabled(args.disable);
				}
			}

			if (const auto& e = get(ConditionParamItem::QuestCondType); e.p1)
			{
				if (ImGui::RadioButton(
						UIL::LS(CommonStrings::Complete, "qts"),
						e.As1<Data::QuestConditionType>() == Data::QuestConditionType::kComplete))
				{
					e.As1<Data::QuestConditionType>() = Data::QuestConditionType::kComplete;
					result                            = true;
				}

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::PackageType); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::PackageType);

				result |= UIPackageTypeSelectorWidget::DrawPackageTypeSelector(
					e.As1<PACKAGE_PROCEDURE_TYPE>());
			}

			if (const auto& e = get(ConditionParamItem::WeatherClass); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::WeatherClass);

				result |= DrawWeatherClassSelector(
					e.As1<WeatherClassificationFlags>());
			}

			if (const auto& e = get(ConditionParamItem::DayOfWeek); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::DayOfWeek);

				result |= UIDayOfWeekSelectorWidget::DrawDayOfWeekSelectorWidget(
					e.As1<RE::Calendar::Day>());
			}

			if (const auto& e = get(ConditionParamItem::LifeState); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::LifeState);

				result |= UILifeStateSelectorWidget::DrawLifeStateSelector(
					e.As1<ActorState::ACTOR_LIFE_STATE>());
			}

			if (const auto& e = get(ConditionParamItem::ActorValue); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::ActorValue);

				result |= UIActorValueSelectorWidget::DrawActorValueSelector(
					e.As1<RE::ActorValue>());
			}

			if (const auto& e = get(ConditionParamItem::CondVarType); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::CondVarType);

				if (!args.hide)
				{
					ImGui::PushItemWidth(ImGui::GetFontSize() * 6.5f);

					result |= UIVariableTypeSelectorWidget::DrawVariableTypeSelectorWidget(
						e.As1<ConditionalVariableType>());

					ImGui::PopItemWidth();

					ImGui::SameLine();
				}
			}

			if (const auto& e = get(ConditionParamItem::CompOper); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::CompOper);

				if (!args.hide)
				{
					ImGui::PushItemWidth(ImGui::GetFontSize() * 3.5f);

					result |= UIComparisonOperatorSelector::DrawComparisonOperatorSelector(
						e.As1<Data::ComparisonOperator>());

					ImGui::PopItemWidth();

					ImGui::SameLine();
				}
			}

			if (const auto& e = get(ConditionParamItem::Float); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::Float);

				if (!args.hide)
				{
					result |= ImGui::InputScalar(
						"##in_flt",
						ImGuiDataType_Float,
						reinterpret_cast<float*>(e.p1),
						nullptr,
						nullptr,
						"%f",
						ImGuiInputTextFlags_EnterReturnsTrue);
				}
			}

			if (const auto& e = get(ConditionParamItem::SunAngle); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::SunAngle);

				if (!args.hide)
				{
					auto value = reinterpret_cast<float*>(e.p1);

					if (ImGui::SliderAngle(
							"##in_ang",
							value,
							-90.0f,
							90.0f,
							"%.0f deg",
							ImGuiSliderFlags_AlwaysClamp))
					{
						result = true;
					}
				}
			}

			if (const auto& e = get(ConditionParamItem::UInt32); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::UInt32);

				if (!args.hide)
				{
					result |= ImGui::InputScalar(
						"##in_ui32",
						ImGuiDataType_U32,
						reinterpret_cast<std::uint32_t*>(e.p1),
						nullptr,
						nullptr,
						"%u",
						ImGuiInputTextFlags_EnterReturnsTrue);
				}
			}

			if (const auto& e = get(ConditionParamItem::Int32); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::Int32);

				if (!args.hide)
				{
					result |= ImGui::InputScalar(
						"##in_i32",
						ImGuiDataType_U32,
						reinterpret_cast<std::uint32_t*>(e.p1),
						nullptr,
						nullptr,
						"%d",
						ImGuiInputTextFlags_EnterReturnsTrue);
				}
			}

			if (const auto& e = get(ConditionParamItem::KeyBindID); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::KeyBindID);

				if (!args.hide)
				{
					result |= UIKeyBindIDSelectorWidget::DrawKeyBindIDSelector(
						e.As1<stl::fixed_string>());
				}
			}

			if (const auto& e = get(ConditionParamItem::Percent); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::Percent);

				if (!args.hide)
				{
					result |= ImGui::SliderFloat(
						UIL::LS(CommonStrings::Percent, "in_prcnt"),
						reinterpret_cast<float*>(e.p1),
						0.0f,
						100.0f,
						"%.2f",
						ImGuiSliderFlags_AlwaysClamp);
				}
			}

			if (const auto& e = get(ConditionParamItem::LightingTemplateInheritanceFlags); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::LightingTemplateInheritanceFlags);

				if (!args.hide)
				{
					UICommon::PushDisabled(args.disable);

					result |= DrawLightingTemplateInheritanceFlags(e.As1<RE::INTERIOR_DATA::Inherit>());

					UICommon::PopDisabled(args.disable);
				}
			}

			if (const auto& e = get(ConditionParamItem::FormAny); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::FormAny);

				if (!args.hide)
				{
					if (args.formFilter)
					{
						m_formPickerAny.SetAllowedTypes(UIFormBrowserCommonFilters::Get(*args.formFilter));
					}
					else
					{
						m_formPickerAny.ClearAllowedTypes();
					}

					const auto label =
						e.p2 ?
							static_cast<Localization::StringID>(reinterpret_cast<std::uintptr_t>(e.p2)) :
							static_cast<Localization::StringID>(CommonStrings::Form);

					UICommon::PushDisabled(args.disable);

					result |= m_formPickerAny.DrawFormPicker(
						"fp_4",
						label,
						e.As1<Game::FormID>());

					UICommon::PopDisabled(args.disable);
				}
			}

			if (m_extraInterface)
			{
				if (const auto& e = get(ConditionParamItem::Extra); e.p1)
				{
					result |= m_extraInterface->DrawConditionParamExtra(e.p1, e.p2);
				}
			}

			GetFormPicker().SetAllowClear(true);
			GetKeywordPicker().SetAllowClear(true);

			ImGui::PopItemWidth();

			ImGui::PopID();

			return result;
		}

		const char* UIConditionParamEditorWidget::GetItemDesc(ConditionParamItem a_item)
		{
			switch (a_item)
			{
			case ConditionParamItem::Form:
			case ConditionParamItem::Keyword:
				{
					if (const auto& e = get(a_item); e.p1)
					{
						GetFormDesc(e.As1<const Game::FormID>());
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;
			case ConditionParamItem::CMENode:
				{
					if (const auto& e = get(a_item); e.p1)
					{
						auto& data = NodeOverrideData::GetCMENodeData();

						auto it = data.find(e.As1<const stl::fixed_string>());
						if (it != data.end())
						{
							return it->second.desc.c_str();
						}
					}

					m_descBuffer[0] = 0x0;
				}
				break;
			case ConditionParamItem::BipedSlot:
				{
					if (const auto& e = get(a_item); e.p1)
					{
						return GetFormKeywordExtraDesc(UIBipedObjectSelectorWidget::GetBipedSlotDesc(
							e.As1<BIPED_OBJECT>()));
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;
			case ConditionParamItem::EquipmentSlot:
				{
					if (const auto& e = get(a_item); e.p1)
					{
						return GetFormKeywordExtraDesc(
							Data::GetSlotName(e.As1<Data::ObjectSlot>()));
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;
			case ConditionParamItem::EquipmentSlotExtra:
				{
					if (const auto& e = get(a_item); e.p1)
					{
						return GetFormKeywordExtraDesc(
							Data::GetSlotName(e.As1<Data::ObjectSlotExtra>()));
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;

			case ConditionParamItem::Furniture:
				{
					if (const auto& e = get(ConditionParamItem::Extra); e.p1)
					{
						auto r = GetFormKeywordExtraDesc(nullptr);

						auto& match = e.As1<Data::configNodeOverrideCondition_t>();

						if (r[0] == 0)
						{
							stl::snprintf(
								m_descBuffer2,
								"LD: %s",
								match.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1) ?
									UIL::LS(CommonStrings::True) :
									UIL::LS(CommonStrings::False));
						}
						else
						{
							stl::snprintf(
								m_descBuffer2,
								"%s, LD: %s",
								r,
								match.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1) ?
									UIL::LS(CommonStrings::True) :
									UIL::LS(CommonStrings::False));
						}

						return m_descBuffer2;
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;

			case ConditionParamItem::CondExtra:
				{
					if (const auto& e = get(a_item); e.p1)
					{
						auto type = e.As1<Data::ExtraConditionType>();

						switch (type)
						{
						case Data::ExtraConditionType::kTimeOfDay:

							if (const auto& f = get(ConditionParamItem::TimeOfDay); f.p1)
							{
								stl::snprintf(
									m_descBuffer,
									"%s [%s]",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									time_of_day_to_desc(f.As1<Data::TimeOfDay>()));

								return m_descBuffer;
							}

							break;

						case Data::ExtraConditionType::kRandomPercent:

							if (const auto& f = get(ConditionParamItem::Percent); f.p1)
							{
								stl::snprintf(
									m_descBuffer,
									"%s [%.2f]",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									f.As1<float>());

								return m_descBuffer;
							}

							break;

						case Data::ExtraConditionType::kNodeMonitor:

							if (const auto& f = get(ConditionParamItem::NodeMon); f.p1)
							{
								stl::snprintf(
									m_descBuffer,
									"%s [%s]",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									get_nodemon_desc(f.As1<std::uint32_t>()));

								return m_descBuffer;
							}

							break;

						case Data::ExtraConditionType::kDayOfWeek:

							if (const auto& f = get(ConditionParamItem::DayOfWeek); f.p1)
							{
								stl::snprintf(
									m_descBuffer,
									"%s [%s]",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									UIDayOfWeekSelectorWidget::day_of_week_to_desc(f.As1<RE::Calendar::Day>()));

								return m_descBuffer;
							}

							break;

						case Data::ExtraConditionType::kLevel:

							{
								const auto& f = get(ConditionParamItem::CompOper);
								const auto& g = get(ConditionParamItem::UInt32);

								stl::snprintf(
									m_descBuffer,
									"%s %s %u",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									f.p1 ? UIComparisonOperatorSelector::comp_operator_to_desc(f.As1<Data::ComparisonOperator>()) : nullptr,
									g.p1 ? g.As1<std::uint32_t>() : 0);

								return m_descBuffer;
							}

							break;

						case Data::ExtraConditionType::kLifeState:

							if (const auto& f = get(ConditionParamItem::LifeState); f.p1)
							{
								stl::snprintf(
									m_descBuffer,
									"%s [%s]",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									UILifeStateSelectorWidget::life_state_to_desc(f.As1<ActorState::ACTOR_LIFE_STATE>()));

								return m_descBuffer;
							}

							break;

						case Data::ExtraConditionType::kActorValue:

							if (const auto& f = get(ConditionParamItem::ActorValue); f.p1)
							{
								const auto& g = get(ConditionParamItem::CompOper);
								const auto& h = get(ConditionParamItem::Float);

								stl::snprintf(
									m_descBuffer,
									"%s [%s %s %f]",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									UIActorValueSelectorWidget::actor_value_to_desc(f.As1<RE::ActorValue>()),
									g.p1 ? UIComparisonOperatorSelector::comp_operator_to_desc(g.As1<Data::ComparisonOperator>()) : nullptr,
									h.p1 ? h.As1<float>() : 0.0f);

								return m_descBuffer;
							}

							break;

						case Data::ExtraConditionType::kSunAngle:

							if (const auto& f = get(ConditionParamItem::SunAngle); f.p1)
							{
								const auto& g = get(ConditionParamItem::CompOper);

								stl::snprintf(
									m_descBuffer,
									"%s %s %.0f deg",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									g.p1 ? UIComparisonOperatorSelector::comp_operator_to_desc(g.As1<Data::ComparisonOperator>()) : nullptr,
									f.As1<float>() * 180.0f / std::numbers::pi_v<float>);

								return m_descBuffer;
							}

							break;
						case Data::ExtraConditionType::kKeyIDToggled:

							if (const auto& f = get(ConditionParamItem::KeyBindID); f.p1)
							{
								stl::snprintf(
									m_descBuffer,
									"%s %s: %s",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									UIL::LS(CommonStrings::ID),
									f.As1<const stl::fixed_string>().c_str());

								return m_descBuffer;
							}

							break;

						case Data::ExtraConditionType::kInteriorAmbientLightLevel:

							{
								const auto& f = get(ConditionParamItem::CompOper);
								const auto& g = get(ConditionParamItem::Float);

								stl::snprintf(
									m_descBuffer,
									"%s %s %f",
									UIConditionExtraSelectorWidget::condition_type_to_desc(type),
									f.p1 ? UIComparisonOperatorSelector::comp_operator_to_desc(f.As1<Data::ComparisonOperator>()) : nullptr,
									g.p1 ? g.As1<float>() : 0);

								return m_descBuffer;
							}

							break;
						}

						if (auto r = UIConditionExtraSelectorWidget::condition_type_to_desc(type))
						{
							return r;
						}
					}
					m_descBuffer[0] = 0x0;
				}
				break;
			case ConditionParamItem::PackageType:
				{
					if (const auto& e = get(a_item); e.p1)
					{
						const auto& type = e.As1<PACKAGE_PROCEDURE_TYPE>();

						return GetFormKeywordExtraDesc(
							type != PACKAGE_PROCEDURE_TYPE::kNone ?
								UIPackageTypeSelectorWidget::procedure_type_to_desc(type) :
								nullptr);
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;
			case ConditionParamItem::WeatherClass:
				{
					if (const auto& e = get(a_item); e.p1)
					{
						const auto& type = e.As1<WeatherClassificationFlags>();

						return GetFormKeywordExtraDesc(weather_class_to_desc(type));
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;
			case ConditionParamItem::CondVarType:
				{
					if (const auto& e = get(a_item); e.p1 && e.p2)
					{
						const auto& type     = e.As1<ConditionalVariableType>();
						const auto& compOper = get(ConditionParamItem::CompOper).As1<Data::ComparisonOperator>();

						switch (type)
						{
						case ConditionalVariableType::kInt32:

							stl::snprintf(
								m_descBuffer,
								"%s %s %d",
								e.As2<const stl::fixed_string>().c_str(),
								UIComparisonOperatorSelector::comp_operator_to_desc(compOper),
								get(ConditionParamItem::Int32).As1<std::int32_t>());

							break;

						case ConditionalVariableType::kFloat:

							stl::snprintf(
								m_descBuffer,
								"%s %s %.3f",
								e.As2<const stl::fixed_string>().c_str(),
								UIComparisonOperatorSelector::comp_operator_to_desc(compOper),
								get(ConditionParamItem::Float).As1<float>());

							break;

						case ConditionalVariableType::kForm:

							stl::snprintf(
								m_descBuffer,
								"%s %s %.8X",
								e.As2<const stl::fixed_string>().c_str(),
								UIComparisonOperatorSelector::comp_operator_to_desc(compOper),
								get(ConditionParamItem::FormAny).As1<Game::FormID>().get());

							break;

						default:
							m_descBuffer[0] = 0x0;
						}
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;
			default:
				m_descBuffer[0] = 0x0;
				break;
			}

			return m_descBuffer;
		}

		bool UIConditionParamEditorWidget::DrawExtra(
			const entry_t&               a_entry,
			ConditionParamItemExtraArgs& a_args,
			ConditionParamItem           a_item)
		{
			bool result = false;

			if (m_extraInterface)
			{
				if (const auto& f = get(ConditionParamItem::Extra); f.p1)
				{
					a_args.p1 = a_entry.p1;
					a_args.p2 = a_entry.p2;
					a_args.p3 = f.p1;

					result = m_extraInterface->DrawConditionItemExtra(
						a_item,
						a_args);
				}
			}

			return result;
		}

		void UIConditionParamEditorWidget::OnChange(
			const entry_t&     a_entry,
			ConditionParamItem a_item)
		{
			if (m_extraInterface)
			{
				if (const auto& f = get(ConditionParamItem::Extra); f.p1)
				{
					ConditionParamItemOnChangeArgs args{
						a_entry.p1,
						a_entry.p2,
						f.p1
					};

					m_extraInterface->OnConditionItemChange(
						a_item,
						args);
				}
			}
		}

		void UIConditionParamEditorWidget::GetFormDesc(Game::FormID a_form)
		{
			if (!a_form)
			{
				m_descBuffer[0] = 0;
				return;
			}

			if (auto info = LookupForm(a_form))
			{
				stl::snprintf(
					m_descBuffer,
					"[%.8X] %s",
					a_form.get(),
					info->form.name.c_str());
			}
			else
			{
				stl::snprintf(
					m_descBuffer,
					"%.8X",
					a_form.get());
			}
		}

		const char* UIConditionParamEditorWidget::GetFormKeywordExtraDesc(
			const char* a_idesc,
			bool        a_race) const noexcept
		{
			Game::FormID a_iform;
			Game::FormID a_ikw;
			Game::FormID a_irace;

			if (const auto& e = get(ConditionParamItem::Form); e.p1)
			{
				a_iform = e.As1<Game::FormID>();
			}

			constexpr const char* kwtag = "KW";

			if (const auto& e = get(ConditionParamItem::Keyword); e.p1)
			{
				a_ikw = e.As1<Game::FormID>();
			}

			if (a_race)
			{
				if (const auto& e = get(ConditionParamItem::Race); e.p1)
				{
					a_irace = e.As1<Game::FormID>();
				}
			}

			if (a_iform && a_ikw)
			{
				if (a_idesc)
				{
					stl::snprintf(
						m_descBuffer,
						"%s, F: %.8X, %s: %.8X",
						a_idesc,
						a_iform.get(),
						kwtag,
						a_ikw.get());
				}
				else
				{
					stl::snprintf(
						m_descBuffer,
						"F: %.8X, %s: %.8X",
						a_iform.get(),
						kwtag,
						a_ikw.get());
				}
			}
			else if (a_iform)
			{
				auto info = LookupForm(a_iform);

				if (a_idesc)
				{
					if (info)
					{
						stl::snprintf(
							m_descBuffer,
							"%s, F: [%.8X] %s",
							a_idesc,
							a_iform.get(),
							info->form.name.c_str());
					}
					else
					{
						stl::snprintf(
							m_descBuffer,
							"%s, F: %.8X",
							a_idesc,
							a_iform.get());
					}
				}
				else
				{
					if (info)
					{
						stl::snprintf(
							m_descBuffer,
							"F: [%.8X] %s",
							a_iform.get(),
							info->form.name.c_str());
					}
					else
					{
						stl::snprintf(
							m_descBuffer,
							"F: %.8X",
							a_iform.get());
					}
				}
			}
			else if (a_ikw)
			{
				auto info = LookupForm(a_ikw);

				if (a_idesc)
				{
					if (info)
					{
						stl::snprintf(
							m_descBuffer,
							"%s, %s: [%.8X] %s",
							a_idesc,
							kwtag,
							a_ikw.get(),
							info->form.name.c_str());
					}
					else
					{
						stl::snprintf(
							m_descBuffer,
							"%s, %s: %.8X",
							a_idesc,
							kwtag,
							a_ikw.get());
					}
				}
				else
				{
					if (info)
					{
						stl::snprintf(
							m_descBuffer,
							"%s: [%.8X] %s",
							kwtag,
							a_ikw.get(),
							info->form.name.c_str());
					}
					else
					{
						stl::snprintf(
							m_descBuffer,
							"%s: %.8X",
							kwtag,
							a_ikw.get());
					}
				}
			}
			else
			{
				if (a_idesc)
				{
					stl::snprintf(
						m_descBuffer,
						"%s",
						a_idesc);
				}
				else
				{
					m_descBuffer[0] = 0;
				}
			}

			if (a_race && a_irace)
			{
				auto info = LookupForm(a_irace);

				char tmp[256];

				if (info)
				{
					stl::snprintf(
						tmp,
						"R: [%.8X] %s",
						a_irace.get(),
						info->form.name.c_str());
				}
				else
				{
					stl::snprintf(
						tmp,
						"R: %.8X",
						a_irace.get());
				}

				if (m_descBuffer[0] == 0)
				{
					stl::snprintf(
						m_descBuffer2,
						"%s",
						tmp);
				}
				else
				{
					stl::snprintf(
						m_descBuffer2,
						"%s, %s",
						m_descBuffer,
						tmp);
				}

				return m_descBuffer2;
			}

			return m_descBuffer;
		}

		void UIConditionParamEditorWidget::Reset()
		{
			for (auto& e : m_entries)
			{
				e = {};
			}

			m_tempFlags = UIConditionParamEditorTempFlags::kNone;
		}

		bool UIConditionParamExtraInterface::DrawConditionItemExtra(
			ConditionParamItem           a_item,
			ConditionParamItemExtraArgs& a_args)
		{
			return false;
		}

		void UIConditionParamExtraInterface::OnConditionItemChange(
			ConditionParamItem                    a_item,
			const ConditionParamItemOnChangeArgs& a_args)
		{
		}

	}
}