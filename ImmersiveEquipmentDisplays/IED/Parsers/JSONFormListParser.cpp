#include "pch.h"

#include "JSONFormListParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFormList_t>::Parse(
			const Json::Value&      a_in,
			Data::configFormList_t& a_out,
			const std::uint32_t     a_version) const
		{
			Parser<Game::FormID> formParser(m_state);

			for (auto& e : a_in)
			{
				Game::FormID tmp;

				if (!formParser.Parse(e, tmp))
				{
					Error("%s: could not parse form ID", __FUNCTION__);
					SetHasErrors();
					continue;
				}

				a_out.emplace_back(tmp);
			}

			return true;
		}

		template <>
		void Parser<Data::configFormList_t>::Create(
			const Data::configFormList_t& a_data,
			Json::Value&                  a_out) const
		{
			Parser<Game::FormID> formParser(m_state);

			for (auto& e : a_data)
			{
				Json::Value v;

				formParser.Create(e, v);

				a_out.append(std::move(v));
			}
		}

	}
}