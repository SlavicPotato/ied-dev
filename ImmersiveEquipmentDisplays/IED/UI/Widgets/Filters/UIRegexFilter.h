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

			[[nodiscard]] virtual bool Test(const std::string& a_haystack) const;

		protected:
			virtual bool ProcessInput(const char* a_label);

		private:
			SetObjectWrapper<except::descriptor> m_lastException;
		};

	}
}