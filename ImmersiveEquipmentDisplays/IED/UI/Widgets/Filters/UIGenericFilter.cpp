#include "pch.h"

#include "UIGenericFilter.h"

namespace IED
{
	namespace UI
	{
		UIGenericFilter::UIGenericFilter() :
			UIFilterBase<std::string>()
		{}

		UIGenericFilter::UIGenericFilter(bool a_isOpen) :
			UIFilterBase<std::string>(a_isOpen)
		{}

		bool UIGenericFilter::ProcessInput(const char* a_label)
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
					m_filter = m_filterBuf.get();
				}
				else
				{
					m_filter.reset();
				}
			}

			return result;
		}

		bool UIGenericFilter::Test(const char* a_haystack) const
		{
			if (!m_filter)
			{
				return true;
			}

			auto end = a_haystack + std::strlen(a_haystack);

			auto p = std::search(
				a_haystack,
				end,
				m_filter->begin(),
				m_filter->end(),
				[](char a_lhs, char a_rhs) {
					return stl::tolower_ascii(a_lhs) == stl::tolower_ascii(a_rhs);
				});

			return (p != end);
		}

		bool UIGenericFilter::Test(const std::string& a_haystack) const
		{
			if (!m_filter)
			{
				return true;
			}

			auto it = std::search(
				a_haystack.begin(),
				a_haystack.end(),
				m_filter->begin(),
				m_filter->end(),
				[](char a_lhs, char a_rhs) {
					return stl::tolower_ascii(a_lhs) == stl::tolower_ascii(a_rhs);
				});

			return (it != a_haystack.end());
		}

	}
}