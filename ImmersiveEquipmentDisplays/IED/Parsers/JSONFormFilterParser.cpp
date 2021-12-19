#include "pch.h"

#include "JSONConfigFormSetParser.h"
#include "JSONFormFilterBaseParser.h"
#include "JSONFormFilterParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configFormFilter_t>::Parse(
			const Json::Value& a_in,
			Data::configFormFilter_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			Parser<Data::configFormFilterBase_t> bparser;

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

			Parser<Data::configFormFilterBase_t> bparser;

			bparser.Create(a_in, data["data"]);

			data["ff_flags"] = stl::underlying(a_in.filterFlags.value);

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::configFormFilter_t>::GetDefault(
			Data::configFormFilter_t& a_out) const
		{
		}
	}
}