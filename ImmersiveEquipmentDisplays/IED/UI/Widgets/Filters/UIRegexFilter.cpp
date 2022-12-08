#include "pch.h"

#include "UIRegexFilter.h"

#include "IED/UI/UICommon.h"

namespace IED
{
	namespace UI
	{
		UIRegexFilter::UIRegexFilter() :
			UIFilterBase<std::regex>()
		{
			SetFlags(ImGuiInputTextFlags_EnterReturnsTrue);
		}

		UIRegexFilter::UIRegexFilter(bool a_isOpen) :
			UIFilterBase<std::regex>(a_isOpen)
		{}

		bool UIRegexFilter::ProcessInput(const char* a_label)
		{
			if (m_nextSetFocus)
			{
				m_nextSetFocus = false;
				ImGui::SetKeyboardFocusHere();
			}

			bool result = ImGui::InputText(
				a_label,
				m_filterBuf.get(),
				FILTER_BUF_SIZE,
				m_inputTextFlags);

			if (result)
			{
				if (m_filterBuf[0] != 0)
				{
					try
					{
						m_filter = std::regex(
							m_filterBuf.get(),
							std::regex_constants::ECMAScript |
								std::regex_constants::optimize |
								std::regex_constants::icase);

						if (m_lastException)
						{
							m_lastException.reset();
						}
					}
					catch (const std::exception& e)
					{
						m_lastException = e;
						m_filter.reset();
					}
				}
				else
				{
					if (m_lastException)
					{
						m_lastException.reset();
					}

					m_filter.reset();
				}

				NextSetFocus();
			}

			if (auto helpText = GetHelpText())
			{
				UICommon::HelpMarker(helpText);
			}

			if (m_lastException)
			{
				ImGui::TextWrapped("%s", m_lastException->what());
				ImGui::Spacing();
			}

			return result;
		}

	}
}