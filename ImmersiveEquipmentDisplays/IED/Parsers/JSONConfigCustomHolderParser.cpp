#include "pch.h"

#include "JSONConfigCustomHolderParser.h"
#include "JSONConfigCustomParser.h"
#include "JSONParsersCommon.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configCustomHolder_t>::Parse(
			const Json::Value&          a_in,
			Data::configCustomHolder_t& a_out) const
		{
			JSON_PARSE_VERSION()

			Parser<Data::configCustom_t> pslot(m_state);

			auto& data = a_in["data"];

			for (auto it = data.begin(); it != data.end(); ++it)
			{
				auto key = it.key().asString();

				auto& v = a_out.data.try_emplace(key).first->second;

				parserDesc_t<Data::configCustom_t> desc[]{
					{ "m", v(ConfigSex::Male) },
					{ "f", v(ConfigSex::Female) }
				};

				for (auto& e : desc)
				{
					if (!pslot.Parse((*it)[e.member], e.data, version))
					{
						return false;
					}
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configCustomHolder_t>::Create(
			const Data::configCustomHolder_t& a_data,
			Json::Value&                      a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configCustom_t> pslot(m_state);

			for (auto& [i, e] : a_data.data)
			{
				parserDescConst_t<Data::configCustom_t> desc[]{
					{ "m", e(ConfigSex::Male) },
					{ "f", e(ConfigSex::Female) }
				};

				auto& v = data[i];

				for (auto& f : desc)
				{
					pslot.Create(f.data, v[f.member]);
				}
			}

			a_out["version"] = CURRENT_VERSION;
		}

	}
}