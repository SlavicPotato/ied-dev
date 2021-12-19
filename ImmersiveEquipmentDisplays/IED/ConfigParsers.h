#pragma once

#include "ConfigCommon.h"

namespace IED
{
	namespace Data
	{
		template <class T>
		static bool ParseForm(const std::string& a_in, T& a_out)
		{
			std::vector<std::string> e;
			StrHelpers::SplitString(a_in, '|', e);

			if (e.size() < 2)
			{
				return false;
			}

			Game::FormID::held_type out;

			std::stringstream ss;
			ss << std::hex;
			ss << e[1];

			ss >> out;

			try
			{
				a_out.emplace(e[0], out);

				return true;
			}
			catch (const std::exception&)
			{
			}

			return false;
		}

	}  // namespace Data
}  // namespace IED
