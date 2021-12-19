#include "pch.h"

#include "JSONConfigStoreCustomParser.h"
#include "JSONConfigStoreParser.h"
#include "JSONConfigStoreSlotParser.h"
#include "JSONConfigStoreNodeOverrideParser.h"

namespace IED
{
	namespace Serialization
	{
		using namespace Data;

		template <>
		bool Parser<Data::configStore_t>::Parse(
			const Json::Value& a_in,
			Data::configStore_t& a_out) const
		{
			std::uint32_t version;

			if (!ParseVersion(a_in, "version", version))
			{
				Error("%s: bad version data", __FUNCTION__);
				return false;
			}

			auto& data = a_in["data"];

			Parser<Data::configStoreSlot_t> sparser;
			Parser<Data::configStoreCustom_t> cparser;
			Parser<Data::configStoreNodeOverride_t> eparser;

			if (!sparser.Parse(data["slot"], a_out.slot))
			{
				return false;
			}

			if (!cparser.Parse(data["custom"], a_out.custom))
			{
				return false;
			}
			
			if (!eparser.Parse(data["transforms"], a_out.transforms))
			{
				return false;
			}

			return true;
		}

		template <>
		void Parser<Data::configStore_t>::Create(
			const Data::configStore_t& a_data,
			Json::Value& a_out) const
		{
			auto& data = (a_out["data"] = Json::Value(Json::ValueType::objectValue));

			Parser<Data::configStoreSlot_t> sparser;
			Parser<Data::configStoreCustom_t> cparser;
			Parser<Data::configStoreNodeOverride_t> eparser;

			sparser.Create(a_data.slot, data["slot"]);
			cparser.Create(a_data.custom, data["custom"]);
			eparser.Create(a_data.transforms, data["transforms"]);

			a_out["version"] = 1u;
		}

		template <>
		void Parser<Data::configStore_t>::GetDefault(Data::configStore_t& a_out) const
		{
		}

	}  // namespace Serialization
}  // namespace IED