#pragma once

#include "ConfigData.h"

namespace IED
{
	namespace Data
	{
		template <class T>
		static bool ParseForm(const std::string& a_in, T& a_out)
		{
			try
			{
				stl::vector<std::string> e;
				stl::split_string(a_in, '|', e);

				if (e.size() < 2)
				{
					return false;
				}

				Game::FormID::held_type out;

				std::stringstream ss;
				ss << std::hex;
				ss << e[1];

				ss >> out;

				a_out.emplace(e[0], out);

				return true;
			}
			catch (const std::exception&)
			{
				return false;
			}
		}

	}
}
