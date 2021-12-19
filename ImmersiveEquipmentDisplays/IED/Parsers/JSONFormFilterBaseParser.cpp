#include "pch.h"

#include "JSONFormFilterBaseParser.h"
#include "JSONConfigFormSetParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFormFilterBase_t>::Parse(
			const Json::Value& a_in,
			Data::configFormFilterBase_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			};

			auto& data = a_in["data"];

			Parser<Data::configFormSet_t> pformset;

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
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configFormSet_t> pformset;

			data["flags"] = stl::underlying(a_in.flags.value);

			if (!a_in.allow.empty())
			{
				pformset.Create(a_in.allow, data["allow"]);
			}

			if (!a_in.deny.empty())
			{
				pformset.Create(a_in.deny, data["deny"]);
			}

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::configFormFilterBase_t>::GetDefault(
			Data::configFormFilterBase_t& a_out) const
		{
		}
	}
}