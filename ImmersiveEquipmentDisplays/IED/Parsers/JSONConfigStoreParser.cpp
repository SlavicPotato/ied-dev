#include "pch.h"

#include "JSONConfigConditionalVariablesHolderParser.h"
#include "JSONConfigStoreCustomParser.h"
#include "JSONConfigStoreNodeOverrideParser.h"
#include "JSONConfigStoreParser.h"
#include "JSONConfigStoreSlotParser.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		static constexpr std::uint32_t CURRENT_VERSION = 1;

		template <>
		bool Parser<Data::configStore_t>::Parse(
			const Json::Value&   a_in,
			Data::configStore_t& a_out) const
		{
			JSON_PARSE_VERSION()

			auto& data = a_in["data"];

			if (auto& v = data["slot"])
			{
				Parser<Data::configStoreSlot_t> sparser(m_state);

				if (!sparser.Parse(v, a_out.slot))
				{
					return false;
				}
			}

			if (auto& v = data["custom"])
			{
				Parser<Data::configStoreCustom_t> cparser(m_state);

				if (!cparser.Parse(v, a_out.custom))
				{
					return false;
				}
			}

			if (auto& v = data["transforms"])
			{
				Parser<Data::configStoreNodeOverride_t> eparser(m_state);

				if (!eparser.Parse(v, a_out.transforms))
				{
					return false;
				}
			}

			if (auto& v = data["cvar"])
			{
				Parser<Data::configConditionalVariablesHolder_t> cvparser(m_state);

				if (!cvparser.Parse(v, a_out.condvars))
				{
					return false;
				}
			}

			return true;
		}

		template <>
		void Parser<Data::configStore_t>::Create(
			const Data::configStore_t& a_data,
			Json::Value&               a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configStoreSlot_t>                  sparser(m_state);
			Parser<Data::configStoreCustom_t>                cparser(m_state);
			Parser<Data::configStoreNodeOverride_t>          eparser(m_state);
			Parser<Data::configConditionalVariablesHolder_t> cvparser(m_state);

			sparser.Create(a_data.slot, data["slot"]);
			cparser.Create(a_data.custom, data["custom"]);
			eparser.Create(a_data.transforms, data["transforms"]);
			cvparser.Create(a_data.condvars, data["cvar"]);

			a_out["version"] = CURRENT_VERSION;
		}

	}
}