#include "pch.h"

#include "UIFormSelectorWidget.h"

#include "IED/UI/PopupQueue/UIPopupQueue.h"
#include "IED/UI/UICommon.h"
#include "IED/UI/UIFormBrowser.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	namespace UI
	{
		UIFormSelectorWidget::UIFormSelectorWidget(
			Controller& a_controller,
			FormInfoFlags a_requiredFlags,
			bool a_restrictTypes,
			bool a_enableFormBrowser,
			bool a_forceBase) :
			UITipsInterface(a_controller),
			UILocalizationInterface(a_controller),
			m_controller(a_controller),
			m_requiredFlags(a_requiredFlags),
			m_restrictTypes(a_restrictTypes),
			m_enableFormBrowser(a_enableFormBrowser),
			m_forceBase(a_forceBase),
			m_state(std::make_shared<state_t>()),
			m_types(std::make_shared<UIFormBrowser::tab_filter_type>())
		{
		}

		bool UIFormSelectorWidget::DrawFormSelector(
			const char* a_label,
			Game::FormID& a_form,
			const char* a_tipText)
		{
			ImGui::PushID("form_sel_w");

			bool result = false;

			if (a_form != m_state->m_lastInputFormID)
			{
				m_state->m_lastInputFormID = a_form;
				m_state->m_bufferedFormID = a_form;

				if (a_form)
				{
					SetInputFormID(m_state, a_form);

					strncpy_s(
						m_state->m_lastInputBuffer,
						m_state->m_inputBuffer,
						sizeof(m_state->m_inputBuffer));

					QueueLookup(a_form);
				}
				else
				{
					m_state->m_inputBuffer[0] = 0;
					m_state->m_lastInputBuffer[0] = 0;
					m_state->m_currentInfo.reset();
				}
			}

			if (GetInputBufferChanged())
			{
				if (auto current = GetFormIDFromInputBuffer();
				    current != m_state->m_bufferedFormID)
				{
					//_DMESSAGE("%s | %X", m_inputBuffer, current);

					m_state->m_bufferedFormID = current;
					if (current)
					{
						if (!m_state->m_currentInfo || m_state->m_currentInfo->form.id != current)
						{
							QueueLookup(current);
						}
					}
					else
					{
						m_state->m_currentInfo.reset();
					}
				}
			}

			if (DrawPopupToggleButtonNoOpen("open_ctx", "context_menu"))
			{
				ImGui::OpenPopup("context_menu");
			}

			bool open_browser = false;

			if (ImGui::BeginPopup("context_menu"))
			{
				if (m_enableFormBrowser)
				{
					if (LCG_MI(CommonStrings::Browse, "1"))
					{
						open_browser = true;
					}
				}

				if (LCG_MI(UIWidgetCommonStrings::CrosshairRef, "2"))
				{
					QueueGetCrosshairRef();
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (m_enableFormBrowser)
			{
				auto& formBrowser = m_controller.UIGetFormBrowser();

				if (open_browser)
				{
					if (formBrowser.Open(false))
					{
						if (m_onOpenFunc)
						{
							m_onOpenFunc(*this, formBrowser);
						}

						formBrowser.SetTabFilter(*m_types);
						formBrowser.SetHighlightForm(a_form);
					}
				}

				if (formBrowser.Draw())
				{
					auto& e = formBrowser.GetSelectedEntry();

					SetInputFormID(m_state, e->formid);

					m_nextGrabKeyboardFocus = true;
				}

				ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
			}

			bool select = false;

			bool valid = IsCurrentValid();

			UICommon::PushDisabled(!valid);

			select |= ImGui::Button(LS(CommonStrings::Select, "ctl_s"));

			UICommon::PopDisabled(!valid);

			ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);

			if (m_nextGrabKeyboardFocus)
			{
				m_nextGrabKeyboardFocus = false;
				ImGui::SetKeyboardFocusHere();
			}

			ImGui::PushID("input");

			select |= ImGui::InputText(
				a_label,
				m_state->m_inputBuffer,
				sizeof(m_state->m_inputBuffer),
				ImGuiInputTextFlags_CharsHexadecimal |
					ImGuiInputTextFlags_CharsUppercase |
					ImGuiInputTextFlags_EnterReturnsTrue);

			ImGui::PopID();

			if (select)
			{
				if (valid)
				{
					auto current = GetFormIDFromInputBuffer();

					if (current != m_state->m_currentInfo->form.id)
					{
						ErrorMessage(LS(UIWidgetCommonStrings::FormNotFoundOrPending));
					}
					else
					{
						m_state->m_lastInputFormID = m_state->m_currentInfo->form.id;

						a_form = GetCurrentFormInfo().id;

						result = true;
					}
				}
				else
				{
					m_nextGrabKeyboardFocus = true;
				}
			}

			if (a_tipText)
			{
				DrawTip(a_tipText);
			}

			if (m_state->m_inputBuffer[0] != 0)
			{
				DrawInfo(a_form);
			}

			ImGui::PopID();

			return result;
		}

		bool UIFormSelectorWidget::DrawFormSelectorMulti()
		{
			if (!m_enableFormBrowser)
			{
				return false;
			}

			ImGui::PushID("form_sel_m_w");

			bool result = false;

			auto& formBrowser = m_controller.UIGetFormBrowser();

			if (ImGui::Button(LS(CommonStrings::Browse, "1")))
			{
				if (formBrowser.Open(true))
				{
					if (m_onOpenFunc)
					{
						m_onOpenFunc(*this, formBrowser);
					}

					formBrowser.SetTabFilter(*m_types);
				}
			}

			result = formBrowser.Draw().result;

			ImGui::PopID();

			return result;
		}

		void UIFormSelectorWidget::Reset()
		{
			m_state->m_bufferedFormID = {};
			m_state->m_lastInputFormID = {};
			m_state->m_currentInfo.reset();
			m_state->m_inputBuffer[0] = 0;
			m_state->m_lastInputBuffer[0] = 0;
		}

		void UIFormSelectorWidget::SetOnFormBrowserOpenFunc(
			on_open_func_t a_func)
		{
			m_onOpenFunc = a_func;
		}

		void UIFormSelectorWidget::SetAllowedTypes(
			std::initializer_list<UIFormBrowser::tab_filter_type::value_type> a_types)
		{
			m_types = std::make_unique<const UIFormBrowser::tab_filter_type>(a_types);
		}

		void UIFormSelectorWidget::SetAllowedTypes(
			const std::shared_ptr<const UIFormBrowser::tab_filter_type>& a_types)
		{
			m_types = a_types;
		}

		const UIFormBrowser::selected_form_list& UIFormSelectorWidget::GetSelectedEntries() const noexcept
		{
			return m_controller.UIGetFormBrowser().GetSelectedEntries();
		}

		void UIFormSelectorWidget::ClearSelectedEntries() noexcept
		{
			m_controller.UIGetFormBrowser().ClearSelectedEntries();
		}

		bool UIFormSelectorWidget::HasType(const formInfo_t& a_info) const
		{
			return m_types->contains(a_info.type) ||
			       (a_info.extraType && m_types->contains(a_info.extraType));
		}

		bool UIFormSelectorWidget::IsEntryValid(const IFormDatabase::entry_t& a_entry) const
		{
			return (a_entry.flags & m_requiredFlags) == m_requiredFlags;
		}

		bool UIFormSelectorWidget::IsCurrentValid() const noexcept
		{
			if (!m_state->m_currentInfo)
			{
				return false;
			}

			auto& info = GetCurrentFormInfo();

			return info.flags.test(m_requiredFlags) &&
			       (!m_restrictTypes || HasType(info));
		}

		void UIFormSelectorWidget::DrawInfo(Game::FormID a_form)
		{
			ImGui::PushID("info_area");

			if (m_state->m_currentInfo)
			{
				bool isValid = IsCurrentValid();

				bool indicateState = a_form != m_state->m_bufferedFormID || !isValid;

				if (indicateState)
				{
					ImGui::PushStyleColor(
						ImGuiCol_Text,
						isValid ?
                            UICommon::g_colorOK :
                            UICommon::g_colorError);

					ImGui::Text("%s", isValid ? LS(CommonStrings::OK) : LS(CommonStrings::Invalid));
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
				}

				auto& base = m_state->m_currentInfo->get_base();
				auto& info = !m_forceBase ?
                                 m_state->m_currentInfo->form :
                                 base;

				if (m_state->m_currentInfo->is_ref())
				{
					ImGui::Text("%s:", LS(CommonStrings::Base));
					ImGui::SameLine();

					char buf[16];

					stl::snprintf(buf, "%.8X##cfid", base.id.get());

					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
					ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.35f, 0.35f, 0.35f, 0.6f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.35f, 0.35f, 0.35f, 0.8f });

					if (ImGui::ButtonEx(buf, { 0, 0 }, ImGuiButtonFlags_DontClosePopups))
					{
						SetInputFormID(m_state, base.id);
					}

					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleVar();

					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
				}

				ImGui::Text("%s: %hhu", LS(CommonStrings::Type), info.type);

				if (auto typeDesc = IFormCommon::GetFormTypeDesc(info.type))
				{
					ImGui::SameLine();
					ImGui::Text("(%s)", typeDesc);
				}

				if (!info.name.empty())
				{
					ImGui::SameLine();
					ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
					ImGui::SameLine();
					ImGui::Text("%s: %s", LS(CommonStrings::Name), info.name.c_str());
				}

				if (indicateState)
				{
					ImGui::PopStyleColor();
				}
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Text, UICommon::g_colorError);
				ImGui::TextUnformatted(LS(UIWidgetCommonStrings::FormNotFound));
				ImGui::PopStyleColor();
			}

			ImGui::PopID();
		}

		void UIFormSelectorWidget::QueueLookup(Game::FormID a_formId)
		{
			m_controller.QueueLookupFormInfo(
				a_formId,
				[state = m_state](std::unique_ptr<formInfoResult_t> a_info) {
					state->m_currentInfo = std::move(a_info);
				});
		}

		void UIFormSelectorWidget::QueueGetCrosshairRef()
		{
			/*m_controller.QueueLookupFormInfoCrosshairRef(
				[state = m_state](std::unique_ptr<formInfoResult_t> a_info) {
					if (a_info)
					{
						stl::snprintf(state->m_inputBuffer, "%.8X", a_info->form.id.get());
						state->m_currentInfo = std::move(a_info);
					}
				});*/

			m_controller.QueueGetCrosshairRef(
				[state = m_state](Game::FormID a_result) {
					if (a_result)
					{
						SetInputFormID(state, a_result);
					}
				});
		}

		Game::FormID UIFormSelectorWidget::GetFormIDFromInputBuffer()
		{
			if (m_state->m_inputBuffer[0] == 0)
			{
				return {};
			}

			char* end = nullptr;

			char tmp[16];
			stl::snprintf(tmp, "0x%s", m_state->m_inputBuffer);

			auto v = std::strtoul(tmp, &end, 0);

			if (end > tmp)
			{
				return v;
			}
			else
			{
				return {};
			}
		}

		void UIFormSelectorWidget::SetInputFormID(
			const std::shared_ptr<state_t>& a_state,
			Game::FormID a_form)
		{
			stl::snprintf(a_state->m_inputBuffer, "%.8X", a_form.get());
		}

		bool UIFormSelectorWidget::GetInputBufferChanged()
		{
			if (std::strncmp(
					m_state->m_lastInputBuffer,
					m_state->m_inputBuffer,
					sizeof(m_state->m_lastInputBuffer)) != 0)
			{
				strncpy_s(
					m_state->m_lastInputBuffer,
					m_state->m_inputBuffer,
					sizeof(m_state->m_inputBuffer));

				return true;
			}
			else
			{
				return false;
			}
		}

		void UIFormSelectorWidget::ErrorMessage(const char* a_text)
		{
			auto& queue = m_controller.UIGetPopupQueue();

			queue.push(
				UIPopupType::Message,
				LS(CommonStrings::Error),
				"%s",
				a_text);
		}

	}
}