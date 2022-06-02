#include "pch.h"

#include "IED/Data.h"
#include "UIConditionParamEditorWidget.h"

#include "IED/Controller/Controller.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		UIConditionParamEditorWidget::UIConditionParamEditorWidget(Controller& a_controller) :
			UIFormLookupInterface(a_controller),
			UILocalizationInterface(a_controller),
			UIConditionExtraSelectorWidget(a_controller),
			UIPackageTypeSelectorWidget(a_controller),
			UIWeatherClassSelectorWidget(a_controller),
			UIComparisonOperatorSelector(a_controller),
			UITimeOfDaySelectorWidget(a_controller),
			m_formPickerForm(a_controller, FormInfoFlags::kNone, true),
			m_formPickerKeyword(a_controller, FormInfoFlags::kNone, true),
			m_formPickerRace(a_controller, FormInfoFlags::kNone, true)
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

			ImGui::PushItemWidth(ImGui::GetFontSize() * -6.0f);

			GetFormPicker().SetAllowClear(!m_tempFlags.test(UIConditionParamEditorTempFlags::kNoClearForm));
			GetKeywordPicker().SetAllowClear(!m_tempFlags.test(UIConditionParamEditorTempFlags::kNoClearKeyword));

			if (const auto& e = get(ConditionParamItem::CondExtra); e.p1)
			{
				auto c = e.As1<Data::ExtraConditionType>();

				if (DrawExtraConditionSelector(
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
					}

					result = true;
				}
			}

			if (const auto& e = get(ConditionParamItem::CMENode); e.p1 && e.p2)
			{
				result |= DrawCMNodeSelector(
					LS(CommonStrings::Node, "ns"),
					e.As1<stl::fixed_string>(),
					NodeOverrideData::GetCMENodeData(),
					static_cast<const stl::fixed_string*>(e.p2));

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::BipedSlot); e.p1)
			{
				result |= DrawBipedObjectSelector(
					LS(CommonStrings::Biped, "bp"),
					e.As1<BIPED_OBJECT>(),
					m_tempFlags.test(UIConditionParamEditorTempFlags::kAllowBipedNone));

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::EquipmentSlot); e.p1)
			{
				result |= DrawObjectSlotSelector(
					LS(CommonStrings::Slot, "ss"),
					e.As1<Data::ObjectSlot>());

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::EquipmentSlotExtra); e.p1)
			{
				result |= DrawObjectSlotSelector(
					LS(CommonStrings::Slot, "sse"),
					e.As1<Data::ObjectSlotExtra>());

				ImGui::Spacing();
			}

			if (const auto& e = get(ConditionParamItem::TimeOfDay); e.p1)
			{
				result |= DrawTimeOfDaySelector(e.As1<Data::TimeOfDay>());
			}

			if (const auto& e = get(ConditionParamItem::Form); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::Form);

				if (!args.hide)
				{
					UICommon::PushDisabled(args.disable);

					result |= m_formPickerForm.DrawFormPicker(
						"fp_1",
						static_cast<Localization::StringID>(CommonStrings::Form),
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
						LS(CommonStrings::Complete, "qts"),
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

				result |= DrawPackageTypeSelector(
					e.As1<PACKAGE_PROCEDURE_TYPE>());
			}

			if (const auto& e = get(ConditionParamItem::WeatherClass); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::WeatherClass);

				result |= DrawWeatherClassSelector(
					e.As1<WeatherClassificationFlags>());
			}

			if (const auto& e = get(ConditionParamItem::CompOper); e.p1)
			{
				ConditionParamItemExtraArgs args;

				result |= DrawExtra(e, args, ConditionParamItem::CompOper);

				if (!args.hide)
				{
					ImGui::PushItemWidth(ImGui::GetFontSize() * 6.5f);

					result |= DrawComparisonOperatorSelector(
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
						return GetFormKeywordExtraDesc(GetBipedSlotDesc(
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
									LS(CommonStrings::True) :
                                    LS(CommonStrings::False));
						}
						else
						{
							stl::snprintf(
								m_descBuffer2,
								"%s, LD: %s",
								r,
								match.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1) ?
									LS(CommonStrings::True) :
                                    LS(CommonStrings::False));
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

						if (type == Data::ExtraConditionType::kTimeOfDay)
						{
							if (const auto& f = get(ConditionParamItem::TimeOfDay); f.p1)
							{
								stl::snprintf(
									m_descBuffer,
									"%s [%s]",
									condition_type_to_desc(type),
									time_of_day_to_desc(f.As1<Data::TimeOfDay>()));

								return m_descBuffer;
							}
						}

						if (auto r = condition_type_to_desc(type))
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
								procedure_type_to_desc(type) :
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

			if (const auto& e = get(ConditionParamItem::Form); e.p1)
			{
				a_iform = e.As1<Game::FormID>();
			}

			const char*        kwtag;
			ConditionParamItem kwparam;

			if (a_race)
			{
				kwtag   = "R";
				kwparam = ConditionParamItem::Race;
			}
			else
			{
				kwtag   = "KW";
				kwparam = ConditionParamItem::Keyword;
			}

			if (const auto& e = get(kwparam); e.p1)
			{
				a_ikw = e.As1<Game::FormID>();
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

	}
}