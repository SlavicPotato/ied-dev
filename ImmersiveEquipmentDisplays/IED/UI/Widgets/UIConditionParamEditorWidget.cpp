#include "pch.h"

#include "IED/Data.h"
#include "UIConditionParamEditorWidget.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIConditionParamEditorWidget::UIConditionParamEditorWidget(Controller& a_controller) :
			UIFormLookupInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_formPickerForm(a_controller, FormInfoFlags::kNone, true),
			m_formPickerKeyword(a_controller, FormInfoFlags::kNone, true)
		{
			GetKeywordPicker().SetAllowedTypes(
				{ BGSKeyword::kTypeID });
		}

		void UIConditionParamEditorWidget::OpenConditionParamEditorPopup()
		{
			m_tempData = {};

			{
				const auto& e = m_entries[stl::underlying(ConditionParamItem::Form)];
				if (e.p1)
				{
					m_tempData.form = *static_cast<Game::FormID*>(e.p1);
				}
			}

			{
				const auto& e = m_entries[stl::underlying(ConditionParamItem::Keyword)];
				if (e.p1)
				{
					m_tempData.keyword = *static_cast<Game::FormID*>(e.p1);
				}
			}

			{
				const auto& e = m_entries[stl::underlying(ConditionParamItem::CMENode)];
				if (e.p1)
				{
					m_tempData.cmeNode = *static_cast<stl::fixed_string*>(e.p1);
				}
			}

			{
				const auto& e = m_entries[stl::underlying(ConditionParamItem::BipedSlot)];
				if (e.p1)
				{
					m_tempData.biped = *static_cast<Biped::BIPED_OBJECT*>(e.p1);
				}
			}

			{
				const auto& e = m_entries[stl::underlying(ConditionParamItem::EquipmentSlot)];
				if (e.p1)
				{
					m_tempData.slot = *static_cast<Data::ObjectSlot*>(e.p1);
				}
			}

			{
				const auto& e = m_entries[stl::underlying(ConditionParamItem::EquipmentSlotExtra)];
				if (e.p1)
				{
					m_tempData.slotExtra = *static_cast<Data::ObjectSlotExtra*>(e.p1);
				}
			}

			ImGui::OpenPopup("match_param_editor");
		}

		bool UIConditionParamEditorWidget::DrawConditionParamEditorPopup()
		{
			bool result = false;

			auto fontSize = ImGui::GetFontSize();

			ImGui::SetNextWindowSizeConstraints({ fontSize * 34.0f, 0.0f }, { 800.0f, 800.0f });

			if (ImGui::BeginPopup("match_param_editor"))
			{
				ImGui::PushItemWidth(fontSize * -5.5f);

				if (const auto& e = get(ConditionParamItem::CMENode); e.p1 && e.p2)
				{
					result |= DrawCMNodeSelector(
						LS(CommonStrings::Node, "ns"),
						m_tempData.cmeNode,
						OverrideNodeInfo::GetCMENodeData(),
						static_cast<const stl::fixed_string*>(e.p2));

					ImGui::Spacing();

					if (result)
					{
						*static_cast<stl::fixed_string*>(e.p1) = m_tempData.cmeNode;
					}
				}

				if (const auto& e = get(ConditionParamItem::BipedSlot); e.p1)
				{
					result |= DrawBipedObjectSelector(
						LS(CommonStrings::Node, "bp"),
						m_tempData.biped);

					ImGui::Spacing();

					if (result)
					{
						*static_cast<std::uint32_t*>(e.p1) = static_cast<std::uint32_t>(m_tempData.biped);
					}
				}

				if (const auto& e = get(ConditionParamItem::EquipmentSlot); e.p1)
				{
					result |= DrawObjectSlotSelector(
						LS(CommonStrings::Slot, "ss"),
						m_tempData.slot);

					ImGui::Spacing();

					if (result)
					{
						*static_cast<Data::ObjectSlot*>(e.p1) = m_tempData.slot;
					}
				}

				if (const auto& e = get(ConditionParamItem::EquipmentSlotExtra); e.p1)
				{
					result |= DrawObjectSlotSelector(
						LS(CommonStrings::Slot, "ss"),
						m_tempData.slotExtra);

					ImGui::Spacing();

					if (result)
					{
						*static_cast<Data::ObjectSlotExtra*>(e.p1) = m_tempData.slotExtra;
					}
				}

				if (const auto& e = get(ConditionParamItem::Form); e.p1)
				{
					if (m_extraInterface)
					{
						if (const auto& f = get(ConditionParamItem::Extra); f.p1)
						{
							result |= m_extraInterface->DrawConditionItemExtra(
								ConditionParamItem::Form,
								e.p1,
								e.p2,
								f.p1);
						}
					}

					result |= m_formPickerForm.DrawFormPicker(
						"fp_1",
						LS(CommonStrings::Form),
						m_tempData.form);

					if (result)
					{
						*static_cast<Game::FormID*>(e.p1) = m_tempData.form;
					}
				}

				if (const auto& e = get(ConditionParamItem::Keyword); e.p1)
				{
					if (m_extraInterface)
					{
						if (const auto& f = get(ConditionParamItem::Extra); f.p1)
						{
							result |= m_extraInterface->DrawConditionItemExtra(
								ConditionParamItem::Keyword,
								e.p1,
								e.p2,
								f.p1);
						}
					}

					result |= m_formPickerKeyword.DrawFormPicker(
						"fp_2",
						LS(CommonStrings::Keyword),
						m_tempData.keyword);

					if (result)
					{
						*static_cast<Game::FormID*>(e.p1) = m_tempData.keyword;
					}
				}

				if (m_extraInterface)
				{
					if (const auto& e = get(ConditionParamItem::Extra); e.p1)
					{
						result |= m_extraInterface->DrawConditionParamExtra(e.p1, e.p2);
					}
				}

				ImGui::PopItemWidth();

				ImGui::EndPopup();
			}

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
						GetFormDesc(*static_cast<const Game::FormID*>(e.p1));
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
						auto& data = OverrideNodeInfo::GetCMENodeData();

						auto it = data.find(*static_cast<const stl::fixed_string*>(e.p1));
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
							*static_cast<Biped::BIPED_OBJECT*>(e.p1)));
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
							Data::GetSlotName(*static_cast<Data::ObjectSlot*>(e.p1)));
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
							Data::GetSlotName(*static_cast<Data::ObjectSlotExtra*>(e.p1)));
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
						auto match = static_cast<Data::configNodeOverrideCondition_t*>(e.p1);

						stl::snprintf(
							m_descBuffer,
							"%s: %s",
							LS(CommonStrings::LayingDown),
							match->flags.test(Data::NodeOverrideConditionFlags::kLayingDown) ?
                                LS(CommonStrings::True) :
                                LS(CommonStrings::False));
					}
					else
					{
						m_descBuffer[0] = 0x0;
					}
				}
				break;
			default:
				m_descBuffer[0] = 0x0;
			}

			return m_descBuffer;
		}

		void UIConditionParamEditorWidget::GetFormDesc(Game::FormID a_form)
		{
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
			const char* a_idesc) const noexcept
		{
			Game::FormID a_iform;
			Game::FormID a_ikw;

			if (const auto& e = get(ConditionParamItem::Form); e.p1)
			{
				a_iform = *static_cast<const Game::FormID*>(e.p1);
			}

			if (const auto& e = get(ConditionParamItem::Keyword); e.p2)
			{
				a_ikw = *static_cast<const Game::FormID*>(e.p2);
			}

			if (a_iform && a_ikw)
			{
				stl::snprintf(
					m_descBuffer,
					"%s, F: %.8X, KW: %.8X",
					a_idesc,
					a_iform.get(),
					a_ikw.get());
			}
			else if (a_iform)
			{
				stl::snprintf(
					m_descBuffer,
					"%s, F: %.8X",
					a_idesc,
					a_iform.get());
			}
			else if (a_ikw)
			{
				stl::snprintf(
					m_descBuffer,
					"%s, KW: %.8X",
					a_idesc,
					a_ikw.get());
			}
			else
			{
				stl::snprintf(
					m_descBuffer,
					"%s",
					a_idesc);
			}

			return m_descBuffer;
		}

		void UIConditionParamEditorWidget::Reset()
		{
			for (auto& e : m_entries)
			{
				e = {};
			}
		}

		bool UIConditionParamExtraInterface::DrawConditionItemExtra(
			ConditionParamItem a_item,
			void* a_p1,
			const void* a_p2,
			void* a_p3)
		{
			return false;
		}

	}
}