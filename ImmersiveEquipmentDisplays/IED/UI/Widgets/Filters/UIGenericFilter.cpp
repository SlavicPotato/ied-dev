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

			bool r;

			if ((r = ImGui::InputText(
					 a_label,
					 m_filterBuf,
					 sizeof(m_filterBuf),
					 m_inputTextFlags)))
			{
				if (stl::strlen(m_filterBuf))
					m_filter = m_filterBuf;
				else
					m_filter.clear();
			}

			return r;
		}

		bool UIGenericFilter::Test(const char* a_haystack) const
		{
			if (!m_filter)
				return true;

			auto end = a_haystack + std::strlen(a_haystack);

			auto p = std::search(
				a_haystack,
				end,
				m_filter->begin(),
				m_filter->end(),
				[](char a_lhs, char a_rhs) {
					return std::tolower(a_lhs) == std::tolower(a_rhs);
				});

			return (p != end);
		}

		bool UIGenericFilter::Test(const std::string& a_haystack) const
		{
			if (!m_filter)
				return true;

			auto it = std::search(
				a_haystack.begin(),
				a_haystack.end(),
				m_filter->begin(),
				m_filter->end(),
				[](char a_lhs, char a_rhs) {
					return std::tolower(a_lhs) == std::tolower(a_rhs);
				});

			return (it != a_haystack.end());
		}

	}
}