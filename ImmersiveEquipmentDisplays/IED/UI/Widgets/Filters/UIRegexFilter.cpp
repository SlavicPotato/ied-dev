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

			bool r;

			if ((r = ImGui::InputText(
					 a_label,
					 m_filterBuf,
					 sizeof(m_filterBuf),
					 m_inputTextFlags)))
			{
				if (stl::strlen(m_filterBuf))
				{
					try
					{
						m_filter = std::regex(
							m_filterBuf,
							std::regex_constants::ECMAScript |
								std::regex_constants::optimize |
								std::regex_constants::icase);

						if (m_lastException)
							m_lastException.clear();
					}
					catch (const std::exception& e)
					{
						m_lastException = e;
						m_filter.clear();
					}
				}
				else
				{
					if (m_lastException)
						m_lastException.clear();

					m_filter.clear();
				}

				NextSetFocus();
			}

			auto helpText = GetHelpText();
			if (helpText)
			{
				UICommon::HelpMarker(helpText);
			}

			if (m_lastException)
			{
				ImGui::TextWrapped("%s", m_lastException->what());
				ImGui::Spacing();
			}

			return r;
		}

		bool UIRegexFilter::Test(const std::string& a_haystack) const
		{
			if (!m_filter)
				return true;

			return std::regex_search(a_haystack, *m_filter);
		}

	}
}