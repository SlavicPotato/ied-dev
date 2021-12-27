#include "pch.h"

#include "JSONConfigFormSetParser.h"
#include "JSONFormFilterBaseParser.h"
#include "JSONFormFilterParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configFormFilter_t>::Parse(
			const Json::Value& a_in,
			Data::configFormFilter_t& a_out) const
		{
			JSON_PARSE_VERSION()

			Parser<Data::configFormFilterBase_t> bparser(m_state);

			auto& data = a_in["data"];

			if (!bparser.Parse(data["data"], a_out))
			{
				return false;
			}

			a_out.filterFlags = static_cast<Data::FormFilterFlags>(
				data.get("ff_flags", stl::underlying(Data::FormFilterFlags::kNone)).asUInt());

			return true;
		}

		template <>
		void Parser<Data::configFormFilter_t>::Create(
			const Data::configFormFilter_t& a_in,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configFormFilterBase_t> bparser(m_state);

			bparser.Create(a_in, data["data"]);

			data["ff_flags"] = stl::underlying(a_in.filterFlags.value);

			a_out["version"] = CURRENT_VERSION;
		}

		template <>
		void Parser<Data::configFormFilter_t>::GetDefault(
			Data::configFormFilter_t& a_out) const
		{
		}
	}
}