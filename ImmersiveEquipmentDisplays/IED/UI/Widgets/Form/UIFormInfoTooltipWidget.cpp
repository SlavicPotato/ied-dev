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
			UIMiscTextInterface(a_controller),
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
				ImGui::Text("%s:", UIL::LS(CommonStrings::FormID));
				ImGui::SameLine();
				ImGui::Text("%.8X", a_info->form.id.get());

				ImGui::Text("%s:", UIL::LS(CommonStrings::Type));

				if (auto typeDesc = UIFormTypeSelectorWidget::form_type_to_desc(a_info->form.type))
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

			if (a_entry.data.state->flags.test(ObjectEntryFlags::kScbLeft))
			{
				ImGui::TextUnformatted(UIL::LS(UIWidgetCommonStrings::LeftScbAttached));
				ImGui::Spacing();
			}

			auto dt = IPerfCounter::delta_us(
				a_entry.data.state->created,
				PerfCounter::Query());

			ImGui::Text("%s: %lld min", UIL::LS(CommonStrings::Age), dt / (1000000ll * 60));

			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}

		void UIFormInfoTooltipWidget::DrawObjectEntryHeaderInfo(
			const formInfoResult_t* a_info,
			const ObjectEntryBase&  a_entry)
		{
			auto& state = a_entry.data.state;

			if (!state)
			{
				return;
			}

			if (!a_entry.IsNodeVisible())
			{
				ImGui::TextColored(
					UICommon::g_colorGreyed,
					"[%s]",
					UIL::LS(CommonStrings::Hidden));

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

				ImGui::Text("%s:", UIL::LS(CommonStrings::Model));
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
						UIL::LS(CommonStrings::Unknown));
				}
			}
		}

		void UIFormInfoTooltipWidget::DrawFormWithInfo(Game::FormID a_form)
		{
			if (ImGui::GetCurrentWindow()->SkipItems)
			{
				return;
			}

			if (a_form)
			{
				const auto result = TextCopyable("%.8X", a_form.get());

				if (result >= TextCopyableResult::kHovered)
				{
					DrawGeneralFormInfoTooltip(a_form);
				}
			}
			else
			{
				ImGui::TextUnformatted(UIL::LS(CommonStrings::None));
			}
		}

		void UIFormInfoTooltipWidget::DrawFormWithInfoWrapped(Game::FormID a_form)
		{
			if (ImGui::GetCurrentWindow()->SkipItems)
			{
				return;
			}

			if (a_form)
			{
				const auto result = TextWrappedCopyable("%.8X", a_form.get());

				if (result >= TextCopyableResult::kHovered)
				{
					DrawGeneralFormInfoTooltip(a_form);
				}
			}
			else
			{
				ImGui::TextWrapped("%s", UIL::LS(CommonStrings::None));
			}
		}

		void UIFormInfoTooltipWidget::DrawGeneralFormInfoTooltip(Game::FormID a_form)
		{
			auto& flc = m_controller.UIGetFormLookupCache();
			DrawGeneralFormInfoTooltip(flc.LookupForm(a_form));
		}

		void UIFormInfoTooltipWidget::DrawGeneralFormInfoTooltip(
			const formInfoResult_t* a_info)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 8, 8 });

			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 50.0f);

			if (a_info)
			{
				DrawFormInfo(a_info->form);

				if (auto& base = a_info->base)
				{
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::Text("%s:", UIL::LS(CommonStrings::Base));
					ImGui::Spacing();

					ImGui::Indent();

					DrawFormInfo(*base);

					ImGui::Unindent();
				}
			}
			else
			{
				ImGui::TextColored(
					UICommon::g_colorWarning,
					"%s",
					UIL::LS(CommonStrings::Unknown));
			}

			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();

			ImGui::PopStyleVar();
		}

		void UIFormInfoTooltipWidget::DrawFormInfo(const formInfo_t& a_info)
		{
			ImGui::Text("%s:", UIL::LS(CommonStrings::FormID));
			ImGui::SameLine();
			ImGui::Text("%.8X", a_info.id.get());

			ImGui::Text("%s:", UIL::LS(CommonStrings::Type));

			if (auto typeDesc = UIFormTypeSelectorWidget::form_type_to_desc(a_info.type))
			{
				ImGui::SameLine();
				ImGui::Text("%s [%hhu]", typeDesc, a_info.type);
			}
			else
			{
				ImGui::SameLine();
				ImGui::Text("%hhu", a_info.type);
			}

			if (!a_info.name.empty())
			{
				ImGui::Text("%s:", UIL::LS(CommonStrings::Name));
				ImGui::SameLine();
				ImGui::Text("%s", a_info.name.c_str());
			}

			ImGui::Text("%s:", UIL::LS(CommonStrings::Flags));
			ImGui::SameLine();
			ImGui::Text("%.8X", a_info.formFlags);
		}
	}
}