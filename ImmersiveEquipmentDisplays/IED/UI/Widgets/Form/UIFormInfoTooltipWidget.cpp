#include "pch.h"

#include "UIFormInfoTooltipWidget.h"

#include "IED/UI/UICommon.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/Controller/IForm.h"
#include "IED/Controller/ObjectManagerData.h"

namespace IED
{
	namespace UI
	{
		UIFormInfoTooltipWidget::UIFormInfoTooltipWidget(
			Localization::ILocalization& a_localization) :
			UILocalizationInterface(a_localization)
		{
		}

		void UIFormInfoTooltipWidget::DrawFormInfoTooltip(
			const formInfoResult_t& a_info,
			const objectEntryBase_t& a_entry)
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);

			ImGui::TextUnformatted(LS(UIWidgetCommonStrings::FormIDColon));
			ImGui::SameLine();
			ImGui::Text("%.8X", a_info.form.id.get());

			if (auto typeDesc = IFormCommon::GetFormTypeDesc(a_info.form.type))
			{
				ImGui::TextUnformatted(LS(UIWidgetCommonStrings::TypeColon));
				ImGui::SameLine();
				ImGui::Text("%s", typeDesc);
			}
			else
			{
				ImGui::TextUnformatted(LS(UIWidgetCommonStrings::TypeColon));
				ImGui::SameLine();
				ImGui::Text("%hhu", a_info.form.type);
			}

			if (a_entry.state->flags.test(ObjectEntryFlags::kScbLeft))
			{
				ImGui::Spacing();
				ImGui::TextUnformatted(LS(UIWidgetCommonStrings::LeftScbAttached));
			}

			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		void UIFormInfoTooltipWidget::DrawObjectEntryHeaderInfo(
			const formInfoResult_t* a_info,
			const objectEntryBase_t& a_entry)
		{
			if (!a_entry.state)
			{
				return;
			}

			if (!a_entry.state->nodes.obj->IsVisible())
			{
				ImGui::TextColored(
					UICommon::g_colorGreyed,
					"%s",
					LS(UIWidgetCommonStrings::HiddenBrackets));

				ImGui::SameLine();
			}

			if (a_info)
			{
				if (a_info->form.name.empty())
				{
					ImGui::TextColored(
						UICommon::g_colorLightBlue,
						"%.8X",
						a_info->form.id.get());
				}
				else
				{
					ImGui::TextColored(
						UICommon::g_colorLightBlue,
						"%s",
						a_info->form.name.c_str());
				}

				if (ImGui::IsItemHovered())
				{
					DrawFormInfoTooltip(*a_info, a_entry);
				}
			}
			else
			{
				ImGui::TextColored(
					UICommon::g_colorLightBlue,
					"%.8X",
					a_entry.state->item.get());

				if (a_entry.state->flags.test(ObjectEntryFlags::kScbLeft))
				{
					UICommon::HelpMarker(LS(UIWidgetCommonStrings::LeftScbAttached));
				}
			}
		}
	}
}