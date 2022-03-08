#include "pch.h"

#include "JSONFormFilterBaseParser.h"
#include "JSONFormFilterBaseSharedParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<std::shared_ptr<Data::configFormFilterBase_t>>::Parse(
			const Json::Value&                             a_in,
			std::shared_ptr<Data::configFormFilterBase_t>& a_out) const
		{
			a_out = std::make_unique<Data::configFormFilterBase_t>();

			Parser<Data::configFormFilterBase_t> parser(m_state);

			return parser.Parse(a_in, *a_out);
		}

		template <>
		void Parser<std::shared_ptr<Data::configFormFilterBase_t>>::Create(
			const std::shared_ptr<Data::configFormFilterBase_t>& a_in,
			Json::Value&                                         a_out) const
		{
			if (a_in)
			{
				Parser<Data::configFormFilterBase_t> parser(m_state);

				parser.Create(*a_in, a_out);
			}
		}

	}
}