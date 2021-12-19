#include "pch.h"

#include "JSONNodeMapParser.h"
#include "JSONNodeMapValueParser.h"

namespace IED
{
	namespace Serialization
	{
		template <>
		bool Parser<Data::NodeMap::map_type>::Parse(
			const Json::Value& a_in,
			Data::NodeMap::map_type& a_out,
			bool a_extraOnly) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			Parser<Data::NodeMap::value_type> parser;

			auto& data = a_in["data"];

			for (auto it = data.begin(); it != data.end(); ++it)
			{
				auto key = it.key().asString();

				if (!Data::NodeMap::ValidateNodeName(key))
				{
					Error("%s: bad node name", __FUNCTION__);
					continue;
				}

				Data::NodeMap::value_type tmp;

				if (!parser.Parse((*it), tmp, version))
				{
					Error("%s: parsing value failed (%s)", __FUNCTION__, key.c_str());
					continue;
				}

				if (a_extraOnly && !tmp.flags.test(Data::NodeDescriptorFlags::kExtra))
				{
					continue;
				}

				a_out.try_emplace(key, std::move(tmp));
			}

			return true;
		}

		template <>
		void Parser<Data::NodeMap::map_type>::Create(
			const Data::NodeMap::map_type& a_data,
			Json::Value& a_out,
			bool a_extraOnly) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::NodeMap::value_type> parser;

			for (auto& e : a_data)
			{
				if (a_extraOnly && !e.second.flags.test(Data::NodeDescriptorFlags::kExtra))
				{
					continue;
				}

				parser.Create(e.second, data[e.first.c_str()]);
			}

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::NodeMap::map_type>::GetDefault(
			Data::NodeMap::map_type& a_out) const
		{}

	}  // namespace Serialization
}  // namespace IED