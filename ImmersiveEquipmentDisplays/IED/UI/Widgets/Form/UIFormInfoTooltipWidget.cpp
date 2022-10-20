#include "pch.h"

#include "UIFormInfoTooltipWidget.h"

#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormInfoCache.h"

#include "IED/UI/Widgets/UIWidgetsCommon.h"

#include "IED/Controller/Controller.h"
#include "IED/Controller/IForm.h"
#include "IED/Controller/ObjectManagerData.h"

namespace IED
{
	namespace UI
	{
		UIFormInfoTooltipWidget::UIFormInfoTooltipWidget(
			Controller& a_controller) :
			UIFormTypeSelectorWidget(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller)
		{
		}

		void UIFormInfoTooltipWidget::DrawFormInfoTooltip(
			const formInfoResult_t* a_info,
			const ObjectEntryBase&  a_entry)
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
			const formInfoResult_t* a_info,
			const ObjectEntryBase&  a_entry)
		{
			auto& state = a_entry.state;

			if (!state)
			{
				return;
			}

			if (!a_entry.IsNodeVisible())
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
					state->formid.get());

				if (ImGui::IsItemHovered())
				{
					DrawFormInfoTooltip(nullptr, a_entry);
				}
			}

			if (state->modelForm && state->formid != state->modelForm)
			{
				ImGui::SameLine(0.0f, 5.0f);
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
				ImGui::SameLine(0.0f, 8.0f);

				ImGui::Text("%s:", LS(CommonStrings::Model));
				ImGui::SameLine();

				auto& flc = m_controller.UIGetFormLookupCache();

				if (auto formInfo = flc.LookupForm(state->modelForm))
				{
					if (!formInfo->form.name.empty())
					{
						ImGui::TextColored(
							UICommon::g_colorLightBlue,
							"%s",
							formInfo->form.name.c_str());
					}
					else
					{
						ImGui::TextColored(
							UICommon::g_colorLightBlue,
							"%.8X",
							state->modelForm.get());
					}

					if (ImGui::IsItemHovered())
					{
						DrawFormInfoTooltip(formInfo, a_entry);
					}
				}
				else
				{
					ImGui::TextColored(
						UICommon::g_colorWarning,
						"%s",
						LS(CommonStrings::Unknown));
				}
			}
		}
	}
}