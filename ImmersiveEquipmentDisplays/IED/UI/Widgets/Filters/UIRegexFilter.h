#pragma once

#include "UIFilterBase.h"

namespace IED
{
	namespace UI
	{
		class UIRegexFilter :
			public UIFilterBase<std::regex>
		{
		public:
			UIRegexFilter();
			UIRegexFilter(bool a_isOpen);

			template <
				class Char,
				class Traits,
				class Allocator>
			[[nodiscard]] bool Test(const std::basic_string<
									Char,
									Traits,
									Allocator>& a_haystack) const;

		protected:
			virtual bool ProcessInput(const char* a_label);

		private:
			std::optional<except::descriptor> m_lastException;
		};

		template <
			class Char,
			class Traits,
			class Allocator>
		bool UIRegexFilter::Test(const std::basic_string<
								 Char,
								 Traits,
								 Allocator>& a_haystack) const
		{
			return m_filter ?
			           std::regex_search(a_haystack, *m_filter) :
                       true;
		}

	}
}