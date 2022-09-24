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
			UIFormTypeSelectorWidget(a_localization),
			UILocalizationInterface(a_localization)
		{
		}

		void UIFormInfoTooltipWidget::DrawFormInfoTooltip(
			const formInfoResult_t*  a_info,
			const ObjectEntryBase& a_entry)
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);

			if (a_info)
			{
				ImGui::TextUnformatted(LS(UIWidgetCommonStrings::FormIDColon));
				ImGui::SameLine();
				ImGui::Text("%.8X", a_info->form.id.get());

				ImGui::TextUnformatted(LS(UIWidgetCommonStrings::TypeColon));

				if (auto typeDesc = form_type_to_desc(a_info->form.type))
				{
					ImGui::SameLine();
					ImGui::TextUnformatted(typeDesc);
				}
				else
				{
					ImGui::SameLine();
					ImGui::Text("%hhu", a_info->form.type);
				}

				ImGui::Spacing();
			}

			if (a_entry.state->flags.test(ObjectEntryFlags::kScbLeft))
			{
				ImGui::TextUnformatted(LS(UIWidgetCommonStrings::LeftScbAttached));
				ImGui::Spacing();
			}

			auto dt = IPerfCounter::delta_us(
				a_entry.state->created,
				PerfCounter::Query());

			ImGui::Text("%s: %lld min", LS(CommonStrings::Age), dt / (1000000ll * 60));

			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		void UIFormInfoTooltipWidget::DrawObjectEntryHeaderInfo(
			const formInfoResult_t*  a_info,
			const ObjectEntryBase& a_entry)
		{
			if (!a_entry.state)
			{
				return;
			}

			if (!a_entry.state->nodes.rootNode->IsVisible())
			{
				ImGui::TextColored(
					UICommon::g_colorGreyed,
					"[%s]",
					LS(CommonStrings::Hidden));

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
					DrawFormInfoTooltip(a_info, a_entry);
				}
			}
			else
			{
				ImGui::TextColored(
					UICommon::g_colorLightBlue,
					"%.8X",
					a_entry.state->formid.get());

				if (ImGui::IsItemHovered())
				{
					DrawFormInfoTooltip(nullptr, a_entry);
				}
			}
		}
	}
}