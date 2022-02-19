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

			if (key == StringHolder::GetSingleton().IED)
			{
				return {};
			}

			if (!IData::GetPluginInfo().GetLookupRef().contains(key))
			{
				return {};
			}

			return key;
		}

	}
}