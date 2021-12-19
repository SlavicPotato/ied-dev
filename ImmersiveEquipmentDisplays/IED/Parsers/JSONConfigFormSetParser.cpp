#include "pch.h"

#include "JSONConfigFormSetParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFormSet_t>::Parse(
			const Json::Value& a_in,
			Data::configFormSet_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Game::FormID> pform;

			for (auto it = a_in.begin(); it != a_in.end(); ++it)
			{
				Game::FormID tmp;
				if (pform.Parse(*it, tmp, a_version))
				{
					a_out.emplace(tmp);
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configFormSet_t>::Create(
			const Data::configFormSet_t& a_in,
			Json::Value& a_out) const
		{
			Parser<Game::FormID> pform;

			for (auto& e : a_in)
			{
				auto tmp = Json::Value();

				pform.Create(e, tmp);

				a_out.append(tmp);
			}
		}

		template <>
		void Parser<Data::configFormSet_t>::GetDefault(
			Data::configFormSet_t& a_out) const
		{
		}
	}
}