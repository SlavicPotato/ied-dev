#pragma once

#include "UIFilterBase.h"

namespace IED
{
	namespace UI
	{
		class UIGenericFilter :
			public UIFilterBase<std::string>
		{
		public:
			UIGenericFilter();
			UIGenericFilter(bool a_isOpen);

			[[nodiscard]] bool Test(const char* a_haystack) const;

			template <class Allocator>
			[[nodiscard]] bool Test(const std::basic_string<
									char,
									std::char_traits<char>,
									Allocator>& a_haystack) const;

		protected:
			virtual bool ProcessInput(const char* a_label);
		};

		template <class Allocator>
		bool UIGenericFilter::Test(const std::basic_string<
								   char,
								   std::char_traits<char>,
								   Allocator>& a_haystack) const
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
					return ::hash::toupper(a_lhs) == ::hash::toupper(a_rhs);
				});

			return (it != a_haystack.end());
		}

	}
}