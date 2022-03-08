#include "pch.h"

#include "JSONNodeMapParser.h"
#include "JSONNodeMapValueParser.h"

namespace IED
{
	namespace Serialization
	{
		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::NodeMap::map_type>::Parse(
			const Json::Value&       a_in,
			Data::NodeMap::map_type& a_out,
			bool                     a_extraOnly) const
		{
			JSON_PARSE_VERSION()

			Parser<Data::NodeMap::value_type> parser(m_state);

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
			Json::Value&                   a_out,
			bool                           a_extraOnly) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::NodeMap::value_type> parser(m_state);

			for (auto& [i, e] : a_data)
			{
				if (a_extraOnly && !e.flags.test(Data::NodeDescriptorFlags::kExtra))
				{
					continue;
				}

				parser.Create(e, data[i.c_str()]);
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}  // namespace Serialization
}  // namespace IED