#include "pch.h"

#include "JSONConfigFormSetParser.h"
#include "JSONFormFilterBaseParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configFormFilterBase_t>::Parse(
			const Json::Value&            a_in,
			Data::configFormFilterBase_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::configFormSet_t> pformset(m_state);

			a_out.flags = static_cast<Data::FormFilterBaseFlags>(
				data.get("flags", stl::underlying(Data::FormFilterBaseFlags::kNone)).asUInt());

			if (!pformset.Parse(data["allow"], a_out.allow, 1u))
			{
				return false;
			}

			if (!pformset.Parse(data["deny"], a_out.deny, 1u))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::configFormFilterBase_t>::Create(
			const Data::configFormFilterBase_t& a_in,
			Json::Value&                        a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configFormSet_t> pformset(m_state);

			data["flags"] = stl::underlying(a_in.flags.value);

			if (!a_in.allow.empty())
			{
				pformset.Create(a_in.allow, data["allow"]);
			}

			if (!a_in.deny.empty())
			{
				pformset.Create(a_in.deny, data["deny"]);
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}