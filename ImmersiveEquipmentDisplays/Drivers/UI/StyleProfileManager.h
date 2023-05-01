#pragma once

#include "IED/Parsers/JSONImGuiStyleHolderParser.h"

#include "IED/Profile/Manager.h"

namespace IED
{
	using StyleProfile = Profile<Data::ImGuiStyleHolder>;

	class StyleProfileManager :
		public ProfileManager<StyleProfile>
	{
	public:
		FN_NAMEPROC("StyleProfileManager");

	private:
		using ProfileManager<StyleProfile>::ProfileManager;
	};
}