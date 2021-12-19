#include "pch.h"

#include "JSONFormListParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFormList_t>::Parse(
			const Json::Value& a_in,
			Data::configFormList_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Game::FormID> formParser;

			for (auto& e : a_in)
			{
				Game::FormID tmp;

				if (!formParser.Parse(e, tmp, a_version))
				{
					continue;
				}

				a_out.emplace_back(tmp);
			}

			return true;
		}

		template <>
		void Parser<Data::configFormList_t>::Create(
			const Data::configFormList_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Game::FormID> formParser;

			for (auto& e : a_data)
			{
				Json::Value v;

				formParser.Create(e, v);

				a_out.append(std::move(v));
			}
		}

		template <>
		void Parser<Data::configFormList_t>::GetDefault(Data::configFormList_t& a_out) const
		{
		}

	}  // namespace Serialization
}  // namespace IED