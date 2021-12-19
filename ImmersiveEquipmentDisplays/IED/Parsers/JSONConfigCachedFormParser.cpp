#include "pch.h"

#include "JSONConfigCachedFormParser.h"
#include "JSONFormParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configCachedForm_t>::Parse(
			const Json::Value& a_in,
			Data::configCachedForm_t& a_out,
			const std::uint32_t a_version) const
		{
			Parser<Game::FormID> formParser;

			Game::FormID tmp;
			if (formParser.Parse(a_in, tmp, a_version))
			{
				a_out = tmp;
			}
			else
			{
				a_out = Data::configCachedForm_t();
			}

			return true;
		}

		template <>
		void Parser<Data::configCachedForm_t>::Create(
			const Data::configCachedForm_t& a_data,
			Json::Value& a_out) const
		{
			Parser<Game::FormID> formParser;

			formParser.Create(a_data.get_id(), a_out);
		}

		template <>
		void Parser<Data::configCachedForm_t>::GetDefault(
			Data::configCachedForm_t& a_out) const
		{
		}

	}  // namespace Serialization
}  // namespace IED