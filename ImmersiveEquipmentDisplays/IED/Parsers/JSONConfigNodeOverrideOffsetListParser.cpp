#include "pch.h"

#include "JSONConfigNodeOverrideOffsetListParser.h"
#include "JSONConfigNodeOverrideOffsetParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::configNodeOverrideOffsetList_t>::Parse(
			const Json::Value& a_in,
			Data::configNodeOverrideOffsetList_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			auto& data = a_in["data"];

			Parser<Data::configNodeOverrideOffset_t> parser(m_state);

			for (auto& e : data)
			{
				Data::configNodeOverrideOffset_t tmp;

				if (!parser.Parse(e, tmp, version))
				{
					continue;
				}

				a_out.emplace_back(std::move(tmp));
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideOffsetList_t>::Create(
			const Data::configNodeOverrideOffsetList_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::arrayValue));

			Parser<Data::configNodeOverrideOffset_t> parser(m_state);

			for (auto& e : a_data)
			{
				Json::Value v;

				parser.Create(e, v);

				data.append(std::move(v));
			}

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::configNodeOverrideOffsetList_t>::GetDefault(
			Data::configNodeOverrideOffsetList_t& a_out) const
		{
		}
	}
}