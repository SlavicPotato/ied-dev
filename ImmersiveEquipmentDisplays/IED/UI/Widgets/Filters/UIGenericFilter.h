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

			[[nodiscard]] virtual bool Test(const char* a_haystack) const;
			[[nodiscard]] virtual bool Test(const std::string& a_haystack) const;

		protected:
			virtual bool ProcessInput(const char* a_label);
		};

	}
}