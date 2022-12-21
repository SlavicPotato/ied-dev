#include "pch.h"

#include "IED/Data.h"
#include "IED/StringHolder.h"

#include "PapyrusCommon.h"

namespace IED
{
	namespace Papyrus
	{
		using namespace Data;

		stl::fixed_string GetKey(const BSFixedString& a_key)
		{
			if (!ValidateString(a_key))
			{
				return {};
			}

			stl::fixed_string key(a_key.c_str());

			if (!ValidateKey(key))
			{
				return {};
			}

			return key;
		}

		bool ValidateKey(const stl::fixed_string& a_key)
		{
			auto& sh = StringHolder::GetSingleton();

			if (a_key == sh.IED)
			{
				return false;
			}

			auto it = std::find(
				sh.papyrusRestrictedPlugins.begin(),
				sh.papyrusRestrictedPlugins.end(),
				a_key);

			if (it != sh.papyrusRestrictedPlugins.end())
			{
				return false;
			}

			if (!IData::GetPluginInfo().GetLookupRef().contains(a_key))
			{
				return false;
			}

			return true;
		}

	}
}