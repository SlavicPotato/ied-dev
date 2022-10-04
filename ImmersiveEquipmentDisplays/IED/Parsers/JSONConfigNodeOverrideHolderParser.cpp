#include "pch.h"

#include "JSONConfigNodeOverrideHolderParser.h"
#include "JSONConfigNodeOverridePlacementParser.h"
#include "JSONConfigNodeOverrideTransformParser.h"

namespace IED
{
	using namespace Data;

	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 2;

		template <>
		bool Parser<Data::configNodeOverrideHolder_t>::Parse(
			const Json::Value&                a_in,
			Data::configNodeOverrideHolder_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			Parser<Data::configNodeOverrideTransform_t> parser(m_state);
			Parser<Data::configNodeOverridePlacement_t> pparser(m_state);

			a_out.flags = static_cast<Data::NodeOverrideHolderFlags>(
				data.get("flags", stl::underlying(Data::NodeOverrideHolderFlags::kNone)).asUInt());

			a_out.flags.clear(Data::NodeOverrideHolderFlags::RandomGenerated);

			auto& vdata = data["data"];

			for (auto it = vdata.begin(); it != vdata.end(); ++it)
			{
				auto& v = a_out.transformData.try_emplace(it.key().asString()).first->second;

				parserDesc_t<Data::configNodeOverrideTransform_t> desc[]{
					{ "m", v(ConfigSex::Male) },
					{ "f", v(ConfigSex::Female) }
				};

				for (auto& e : desc)
				{
					if (!parser.Parse((*it)[e.member], e.data, version))
					{
						return false;
					}
				}
			}

			auto& pdata = data["pdata"];

			for (auto it = pdata.begin(); it != pdata.end(); ++it)
			{
				auto& v = a_out.placementData.try_emplace(it.key().asString()).first->second;

				parserDesc_t<Data::configNodeOverridePlacement_t> desc[]{
					{ "m", v(ConfigSex::Male) },
					{ "f", v(ConfigSex::Female) }
				};

				for (auto& e : desc)
				{
					if (!pparser.Parse((*it)[e.member], e.data, version))
					{
						return false;
					}
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configNodeOverrideHolder_t>::Create(
			const Data::configNodeOverrideHolder_t& a_data,
			Json::Value&                            a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configNodeOverrideTransform_t> parser(m_state);
			Parser<Data::configNodeOverridePlacement_t> pparser(m_state);

			data["flags"] = a_data.flags.underlying();

			if (!a_data.transformData.empty())
			{
				auto& vdata = (data["data"] = Json::Value(Json::ValueType::objectValue));

				for (auto& [i, e] : a_data.transformData)
				{
					parserDescConst_t<Data::configNodeOverrideTransform_t> desc[]{
						{ "m", e(ConfigSex::Male) },
						{ "f", e(ConfigSex::Female) }
					};

					auto& v = vdata[i];

					for (auto& f : desc)
					{
						parser.Create(f.data, v[f.member]);
					}
				}
			}

			if (!a_data.placementData.empty())
			{
				auto& pdata = (data["pdata"] = Json::Value(Json::ValueType::objectValue));

				for (auto& [i, e] : a_data.placementData)
				{
					parserDescConst_t<Data::configNodeOverridePlacement_t> desc[]{
						{ "m", e(ConfigSex::Male) },
						{ "f", e(ConfigSex::Female) }
					};

					auto& v = pdata[i];

					for (auto& f : desc)
					{
						pparser.Create(f.data, v[f.member]);
					}
				}
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}