#pragma once

#include <ext/InteriorData.h>

#include "UILightingTemplateInheritanceFlagsWidgetStrings.h"

#include "IED/UI/UILocalizationInterface.h"

namespace IED
{
	class Controller;

	namespace UI
	{
		class UILightingTemplateInheritanceFlagsWidget
		{
		public:
			UILightingTemplateInheritanceFlagsWidget() = default;

			bool DrawLightingTemplateInheritanceFlags(RE::INTERIOR_DATA::Inherit& a_flags) const;

			const char* lt_inheritance_flags_to_desc(stl::flag<RE::INTERIOR_DATA::Inherit> a_flags) const;

		private:
			mutable std::string m_buf;
		};
	}

}
