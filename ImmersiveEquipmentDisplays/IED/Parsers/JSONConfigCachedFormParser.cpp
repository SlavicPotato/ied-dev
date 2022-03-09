#include "pch.h"

#include "JSONConfigCachedFormParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configCachedForm_t>::Parse(
			const Json::Value&        a_in,
			Data::configCachedForm_t& a_out) const
		{
			Parser<Game::FormID> formParser(m_state);

			Game::FormID tmp;

			bool result = formParser.Parse(a_in, tmp);

			a_out = tmp;

			return result;
		}

		template <>
		void Parser<Data::configCachedForm_t>::Create(
			const Data::configCachedForm_t& a_data,
			Json::Value&                    a_out) const
		{
			Parser<Game::FormID> formParser(m_state);

			formParser.Create(a_data.get_id(), a_out);
		}

	}
}